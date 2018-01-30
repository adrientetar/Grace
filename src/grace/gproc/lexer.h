/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <string>

#include "types.h"

class LexerException : public PosException { using PosException::PosException; };

class Lexer {
public:
    Lexer(const std::wstring& text);
    Token::Token next();

private:
    void scan_integer_();
    Token::Token tokenize_alpha_();
    Token::Token tokenize_comment_();
    Token::Token tokenize_number_();

    unsigned pos_;
    const std::wstring& text_;
    size_t text_length_;
};
