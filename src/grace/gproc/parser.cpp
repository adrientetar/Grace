/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "parser.h"

void Args::set(TokenType_ kind, float value)
{
    switch (kind)
    {
    case TokenType_::X: x = value; break;
    case TokenType_::Y: y = value; break;
    case TokenType_::Z: z = value; break;
    case TokenType_::S: s = value; break;
    case TokenType_::F: f = value; break;
    case TokenType_::I: i = value; break;
    case TokenType_::J: j = value; break;
    case TokenType_::H: h = value; break;
    case TokenType_::P: p = value; break;
    case TokenType_::E: e = value; break;

    default: /* unreachable */;
    }
}

// TODO: generalize the parser to work with a std::istream
// instead of string?
//
// std::wstringstream stream(L"foo");
// std::istream_iterator<std::wstring, wchar_t> it(stream);
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

std::vector<Line> Parser::parse()
{
    std::vector<Line> commands;

    advance_lexer_();
    advance_lexer_();
    while (next_token_.type != TokenType_::EndOfFile)
    {
        // A line of G-code is either:
        // a program number (O)
        if (cur_token_.type == TokenType_::O)
        {
            commands.emplace_back(fetch_unsigned_());
            continue;
        }
        // or:
        // a line number? (N)
        std::optional<unsigned> lineNumber = std::nullopt;
        if (cur_token_.type == TokenType_::N)
        {
            lineNumber = fetch_unsigned_();
        }
        // command (word)
        auto cmd = fetch_command_();
        // args (word+)
        auto args = fetch_args_();
        // TODO: give Command a ctor and we can emplace_back for sign
        commands.emplace_back(Command { cmd, args, lineNumber });
    }
    return commands;
}

void Parser::advance_lexer_()
{
    do {
        cur_token_ = next_token_;
        next_token_ = lexer_->next();

        // actually maybe we shouldn't throw on unknown here so that we get better
        // error checks "expected xxx"
        //if (cur_token_.type == TokenType_::Unknown)
        //{
        //    throw ParserException(
        //        std::string("Unknown token"),
        //        cur_token_.start, cur_token_.length);
        //}
    } while (cur_token_.type != TokenType_::EndOfFile &&
             (cur_token_.type == TokenType_::Comment ||
              cur_token_.type == TokenType_::Percent)); // we should handle comments here for round tripping
}

Args Parser::fetch_args_()
{
    Args args;
    std::optional<Word> arg;
    while ((arg = fetch_argument_()) != std::nullopt)
    {
        args.set(arg->kind, arg->value);
    }

    return args;
}

std::optional<Word> Parser::fetch_argument_(bool optional/* = true*/)
{
    if (!(cur_token_.type == TokenType_::X ||
          cur_token_.type == TokenType_::Y ||
          cur_token_.type == TokenType_::Z ||
          cur_token_.type == TokenType_::S ||
          cur_token_.type == TokenType_::F ||
          cur_token_.type == TokenType_::I ||
          cur_token_.type == TokenType_::J ||
          cur_token_.type == TokenType_::H ||
          cur_token_.type == TokenType_::P ||
          cur_token_.type == TokenType_::E))
    {
        if (optional) return {};
        throw ParserException(
            std::string("Expected argument (X Y Z S F I J H P E), not ") + TokenType_ToString(cur_token_.type),
            cur_token_.start, cur_token_.length);
    };
    return get_word_();
}

Word Parser::fetch_command_()
{
    if (!(cur_token_.type == TokenType_::G ||
          cur_token_.type == TokenType_::M ||
          cur_token_.type == TokenType_::T))
    {
        throw ParserException(
            std::string("Expected command (G M T), not ") + TokenType_ToString(cur_token_.type),
            cur_token_.start, cur_token_.length);
    };
    return get_word_();
}

unsigned Parser::fetch_unsigned_()
{
    if (next_token_.type == TokenType_::Number)
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

inline Word Parser::get_word_()
{
    if (next_token_.type == TokenType_::Number)
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
        std::string("Expected <unsigned> after ") + TokenType_ToString(cur_token_.type),
        next_token_.start, next_token_.length);
}
