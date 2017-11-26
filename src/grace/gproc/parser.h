/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <exception>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "lexer.h"

class ParserException : public std::exception
{
public:
    ParserException(std::string msg, unsigned position, unsigned length)
            : message_(msg), position_(position), length_(length) { }
    unsigned position() { return position_; }
    unsigned length() { return length_; }
    const char* what() const throw()
    {
        return message_.c_str();
    }
private:
    std::string message_;
    unsigned position_;
    unsigned length_;
};

struct Args {
    void set(TokenType_ kind, float value);

    std::optional<float> x = std::nullopt;
    std::optional<float> y = std::nullopt;
    std::optional<float> z = std::nullopt;
    std::optional<float> s = std::nullopt;
    std::optional<float> t = std::nullopt;
    std::optional<float> f = std::nullopt;
    std::optional<float> i = std::nullopt;
    std::optional<float> j = std::nullopt;
    std::optional<float> h = std::nullopt;
    std::optional<float> p = std::nullopt;
    std::optional<float> e = std::nullopt;
};

struct Word {
    TokenType_ kind;
    float value;
};

struct Command {
    Word instr;
    Args args;
    std::optional<unsigned> line_number;
};

using PgmNumber = unsigned;
using Line = std::variant<PgmNumber, Command>;

class Parser {
public:
    Parser(const std::wstring& text);
    ~Parser();
    std::vector<Line> parse();
    Token peek();

private:
    void advance_lexer_();
    Args fetch_args_();
    std::optional<Word> fetch_argument_(bool optional = true);
    Word fetch_command_();
    unsigned fetch_unsigned_();
    inline Word get_word_();

    Lexer* lexer_;
    Token cur_token_;
    Token next_token_;
    const std::wstring& text_;
};
