/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cwctype>

#include "lexer.h"

Lexer::Lexer(const std::wstring& text)
    : text_(text)
{
    pos_ = 0;

    text_length_ = text.length();
}

Token::Token Lexer::next()
{
    wchar_t c;
    while(pos_ < text_length_)
    {
        c = text_[pos_];
        ++pos_;

        // comma is mentioned in the spec list, but unclear
        // what it's for
        if (c == '+' || c == '-' || c == '.' || iswdigit(c))
        {
            return tokenize_number_();
        }
        else if (iswalpha(c))
        {
            return tokenize_alpha_();
        }
        else if (c == '(')
        {
            return tokenize_comment_();
        }
        else if (c == '=')
        {
            return Token::Token {
                pos_ - 1,
                1,
                Token::Equal,
            };
        }
        else if (c == '%')
        {
            return Token::Token {
                pos_ - 1,
                1,
                Token::Percent,
            };
        }
        else if (c == ':')
        {
            return Token::Token {
                pos_ - 1,
                1,
                Token::AlignmentChar,
            };
        }
        else if (c == '/')
        {
            return Token::Token {
                pos_ - 1,
                1,
                Token::OptBlockSkip,
            };
        }
        else if (c == '\u0009' || /* TAB */
                 c == '\u000A' || /* LF/NL */
                 c == '\u000D' || /* CR */
                 c == '\u007F' || /* DEL */
                 c == ' ')
        {
            // non-printing chars, only LF has semantics
            if (c != '\u000A') continue;
            return Token::Token {
                pos_ - 1,
                1,
                Token::EndOfBlock,
            };
        }
        else
        {
            return Token::Token {
                pos_ - 1,
                1,
                Token::Unknown,
            };
        }
    }

    return Token::Token {
        pos_,
        0,
        Token::EndOfFile,
    };
}

void Lexer::scan_integer_()
{
    wchar_t c;
    while (pos_ < text_length_)
    {
        c = text_[pos_];
        if (!iswdigit(c)) {
            break;
        }
        ++pos_;
    }
}

Token::Token Lexer::tokenize_alpha_()
{
    Token::Type kind;

    switch (text_[pos_-1])
    {
    case 'N': kind = Token::N; break;
    case 'O': kind = Token::O; break;

    case 'G': kind = Token::G; break;

    case 'X': kind = Token::X; break;
    case 'Y': kind = Token::Y; break;
    case 'Z': kind = Token::Z; break;
    case 'U': kind = Token::U; break;
    case 'V': kind = Token::V; break;
    case 'W': kind = Token::W; break;
    case 'P': kind = Token::P; break;
    case 'Q': kind = Token::Q; break;
    case 'R': kind = Token::R; break;
    case 'A': kind = Token::A; break;
    case 'B': kind = Token::B; break;
    case 'C': kind = Token::C; break;

    case 'I': kind = Token::I; break;
    case 'J': kind = Token::J; break;
    case 'K': kind = Token::K; break;

    case 'E': kind = Token::E; break;
    case 'F': kind = Token::F; break;

    case 'S': kind = Token::S; break;

    case 'D': kind = Token::D; break;
    case 'T': kind = Token::T; break;

    case 'M': kind = Token::M; break;

    default: kind = Token::Unknown;
    }

    return Token::Token {
        pos_-1,
        1,
        kind,
    };
}

Token::Token Lexer::tokenize_comment_()
{
    bool done;
    unsigned start = pos_ - 1;

    while (pos_ < text_length_) {
        if (text_[pos_] == ':' ||
            text_[pos_] == '%') {
            throw LexerException(
                std::string("Illegal char in comment: ") + std::to_string(text_[pos_]), pos_, 1);
        }
        done = text_[pos_] == ')';
        ++pos_;
        if (done) break;
    }

    return Token::Token {
        start,
        pos_-start,
        Token::Comment,
    };
}

Token::Token Lexer::tokenize_number_()
{
    //auto kind = Token::Integer;
    unsigned start = pos_ - 1;

    scan_integer_();
    if (pos_ < text_length_ && text_[pos_] == '.' &&
            /* leading dot means we can't have another */
            text_[start] != '.') {
        ++pos_;
        if (pos_ < text_length_ && iswdigit(text_[pos_])) {
            scan_integer_();
        }
        //kind = Token::Decimal;
    }

    return Token::Token {
        start,
        pos_-start,
        Token::Number,
    };
}
