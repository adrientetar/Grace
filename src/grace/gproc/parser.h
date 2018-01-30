/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "lexer.h"
#include "types.h"

class ParserException : public PosException { using PosException::PosException; }; // private inheritance markup?


class Parser {
public:
    Parser(const std::wstring& text);
    ~Parser();
    Program parse();

private:
    void add_word_no_dupl_(std::vector<Word>& words, WordSet& rec_words);
    void add_word_no_type_dupl_(std::vector<Word>& words, TokenSet& rec_types);
    void advance_lexer_();
    Block fetch_block_();
    std::wstring fetch_comment_();
    Header fetch_header_();
    unsigned fetch_unsigned_();
    Word fetch_word_();

    Lexer* lexer_;
    Token::Token cur_token_;
    Token::Token next_token_;
    const std::wstring& text_;
};
