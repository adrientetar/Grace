/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "parser.h"

Parser::Parser(const std::wstring& text)
    : lexer_(new Lexer(text)), text_(text)
{
    /* next_token_ */
    /* cur_token_  */
}

Parser::~Parser()
{
    delete lexer_;
}

Program Parser::parse()
{
    Program program;
    advance_lexer_();
    advance_lexer_();

    program.header = fetch_header_();
    while (next_token_.type != Token::EndOfFile)
    {
        program.blocks.emplace_back(fetch_block_());
    }

    return program;
}

Header Parser::fetch_header_()
{
    Header header;

    if (cur_token_.type != Token::Percent)
    {
        throw ParserException(
            std::string("Expected % starting header, not") + TokenType_ToString(cur_token_.type),
            cur_token_.start, cur_token_.length);
    }
    if (next_token_.type == Token::Comment)
    {
        header.identifier = fetch_comment_();
    }
    else if (next_token_.type == Token::Number)
    {
        header.identifier = fetch_unsigned_();
    }
    else
    {
        advance_lexer_();
    }

    if (!(cur_token_.type == Token::EndOfBlock ||
          cur_token_.type == Token::EndOfFile))
    {
        throw ParserException(
            std::string("Expected <newline> ending header, not") + TokenType_ToString(cur_token_.type),
            cur_token_.start, cur_token_.length);
    }
    advance_lexer_();
    return header;
}

Block Parser::fetch_block_()
{
    Block block;
    TokenSet rec_types;
    WordSet rec_words;

    if (cur_token_.type == Token::N)
    {
        block.number = BlockNumber(fetch_unsigned_());
    }
    // prep words
    while (cur_token_.type == Token::G)
    {
        add_word_no_dupl_(block.data_words, rec_words);
    }
    auto hasDimension = false;
    // dimension words
    while (cur_token_.type == Token::X ||
           cur_token_.type == Token::Y ||
           cur_token_.type == Token::Z ||
           cur_token_.type == Token::U ||
           cur_token_.type == Token::V ||
           cur_token_.type == Token::W ||
           cur_token_.type == Token::P ||
           cur_token_.type == Token::Q ||
           cur_token_.type == Token::R ||
           cur_token_.type == Token::A ||
           cur_token_.type == Token::B ||
           cur_token_.type == Token::C)
    {
        add_word_no_type_dupl_(block.data_words, rec_types);
        hasDimension = true;
    }
    if (hasDimension)
    {
        // interpolation words
        while (cur_token_.type == Token::I ||
               cur_token_.type == Token::J ||
               cur_token_.type == Token::K)
        {
            add_word_no_type_dupl_(block.data_words, rec_types);
        }
        // advance words
        while (cur_token_.type == Token::E ||
               cur_token_.type == Token::F)
        {
            add_word_no_type_dupl_(block.data_words, rec_types);
        }
    }
    // spin word
    if (cur_token_.type == Token::S)
    {
        block.data_words.emplace_back(fetch_word_());
    }
    // tool words
    while (cur_token_.type == Token::D ||
           cur_token_.type == Token::T)
    {
        add_word_no_type_dupl_(block.data_words, rec_types);
    }
    // aux words
    while (cur_token_.type == Token::M)
    {
        add_word_no_dupl_(block.data_words, rec_words);
    }

    if (!(cur_token_.type == Token::EndOfBlock ||
          cur_token_.type == Token::EndOfFile))
    {
        throw ParserException(
            std::string("Expected <newline> ending block, not ") + TokenType_ToString(cur_token_.type),
            cur_token_.start, cur_token_.length);
    }
    advance_lexer_();
    return block;
}

void Parser::add_word_no_dupl_(std::vector<Word>& words, WordSet& rec_words)
{
    auto start = cur_token_.start;
    auto word = fetch_word_();
    if (!rec_words.emplace(word).second)
    {
        throw ParserException(
            std::string("Illegal duplicate ") + Word_ToString(word) + " within block",
            start, cur_token_.start - start);
    }
    words.emplace_back(word);
}

void Parser::add_word_no_type_dupl_(std::vector<Word>& words, TokenSet& rec_types)
{
    if (!rec_types.emplace(cur_token_.type).second)
    {
        throw ParserException(
            std::string("Cannot specify ") + TokenType_ToString(cur_token_.type) + " twice within a block",
            cur_token_.start, cur_token_.length);
    }
    words.emplace_back(fetch_word_());
}

void Parser::advance_lexer_()
{
    do {
        cur_token_ = next_token_;
        next_token_ = lexer_->next();
    }
    // TODO: handle and store comments
    while (cur_token_.type == Token::Comment);
}

unsigned Parser::fetch_unsigned_()
{
    if (next_token_.type == Token::Number)
    {
        try {
            auto ret = (unsigned) std::stoi(text_.substr(
                next_token_.start, next_token_.length));
            /* advance lexer afterward so we can have a unique exc path */
            advance_lexer_(); advance_lexer_();
            return ret;
        }
        catch (...) { /* fallthrough */ }
    }
    throw ParserException(
        std::string("Expected <unsigned> after ") + TokenType_ToString(cur_token_.type),
        next_token_.start, next_token_.length);
}

std::wstring Parser::fetch_comment_()
{
    if (next_token_.type == Token::Comment)
    {
        try {
            auto ret = text_.substr(
                next_token_.start, next_token_.length);
            /* advance lexer afterward so we can have a unique exc path */
            advance_lexer_(); advance_lexer_();
            return ret;
        }
        catch (...) { /* fallthrough */ }
    }
    throw ParserException(
        std::string("Expected <comment> after ") + TokenType_ToString(cur_token_.type),
        next_token_.start, next_token_.length);
}

Word Parser::fetch_word_()
{
    if (next_token_.type == Token::Number)
    {
        auto kind = cur_token_.type;
        try
        {
            // double or float?
            float value = std::stof(text_.substr(
                next_token_.start, next_token_.length));
            /* advance lexer afterward so we can have a unique exc path */
            advance_lexer_(); advance_lexer_();
            return Word { kind, value };
        }
        catch (...) { /* fallthrough */ }
    }

    throw ParserException(
        std::string("Expected <number> after ") + TokenType_ToString(cur_token_.type),
        next_token_.start, next_token_.length);
}
