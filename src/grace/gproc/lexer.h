/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <iostream>
#include <string>

enum class TokenType_ {
    // commands
    G,
    T,
    N,
    O,

    // arguments
    X,
    Y,
    Z,
    F,
    M,
    S,
    R,
    H,
    P,
    I,
    J,
    E,

    Number,

    Minus,
    Percent,

    Comment,

    EndOfFile,
    Unknown
};

struct Token {
    //bool operator == (const Token& v) const;
    //bool operator != (const Token& v) const;
    //std::ostream& operator<< (std::ostream& stream, const Token& t);
    unsigned start;
    unsigned length;
    TokenType_ type;
};

inline const char* TokenType_ToString(TokenType_ tt)
{
    switch (tt)
    {
        case TokenType_::G:   return "G";
        case TokenType_::T:   return "T";
        case TokenType_::N:   return "N";
        case TokenType_::O:   return "O";

        case TokenType_::X:   return "X";
        case TokenType_::Y:   return "Y";
        case TokenType_::Z:   return "Z";
        case TokenType_::F:   return "F";
        case TokenType_::M:   return "M";
        case TokenType_::S:   return "S";
        case TokenType_::R:   return "R";
        case TokenType_::H:   return "H";
        case TokenType_::P:   return "P";
        case TokenType_::I:   return "I";
        case TokenType_::J:   return "J";
        case TokenType_::E:   return "E";

        case TokenType_::Number:   return "Number";

        case TokenType_::Minus:   return "Minus";
        case TokenType_::Percent:   return "Percent";
        case TokenType_::Comment:   return "Comment";
        case TokenType_::EndOfFile:   return "EndOfFile";
        case TokenType_::Unknown:   return "Unknown";
        default:      return "";
    }
}

inline std::ostream& operator<<(std::ostream& stream,
                         const Token& t) {
    return stream << TokenType_ToString(t.type) << "(" << t.start << "," << t.length << ")" << std::endl;
}

class Lexer {
public:
    Lexer(const std::wstring& text);
    Token next();
    Token peek();

private:
    void scan_integer_();
    Token tokenize_alpha_();
    Token tokenize_comment_(bool untilEol = false);
    Token tokenize_number_();

    unsigned pos_;
    const std::wstring& text_;
    size_t text_length_;
};
