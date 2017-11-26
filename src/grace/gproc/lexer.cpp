/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cwctype>

#include "lexer.h"

// TODO: generalize the parser to work with a std::istream
// instead of string?
//
// std::wstringstream stream(L"foo");
// std::istream_iterator<std::wstring, wchar_t> it(stream);
Lexer::Lexer(const std::wstring& text)
    : text_(text)
{
    pos_ = 0;

    text_length_ = text.length();
}

// TODO: word boundary between matches? seems some gcode files omit spaces though

Token Lexer::next()
{
    wchar_t c;
    while(pos_ < text_length_)
    {
        c = text_[pos_];
        ++pos_;
        if (iswspace(c)) continue;

        if (c == '-' || iswdigit(c))
        {
            return tokenize_number_();
        }
        else if (iswalpha(c))
        {
            return tokenize_alpha_();
        }
        else if (c == ';')
        {
            return tokenize_comment_(true);
        }
        else if (c == '(')
        {
            return tokenize_comment_();
        }
        else if (c == '%')
        {
            // ignore percent signs
            continue;//return Token {
            //    pos_ - 1,
            //    1,
            //    TokenType::Percent,
            //}
        }
        //else if (c == '-')
        //{
        //    return Token {
        //        pos_ - 1,
        //        1,
        //        TokenType_::Minus,
        //    };
        //}
        else
        {
            return Token {
                pos_ - 1,
                1,
                TokenType_::Unknown,
            };
        }
    }

    return Token {
        pos_,
        0,
        TokenType_::EndOfFile,
    };
}

Token Lexer::peek()
{
    unsigned pos = pos_;
    Token t = next();
    pos_ = pos;
    return t;
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

Token Lexer::tokenize_alpha_()
{
    TokenType_ kind;

    switch (text_[pos_-1])
    {
    case 'G': kind = TokenType_::G; break;
    case 'M': kind = TokenType_::M; break;
    case 'T': kind = TokenType_::T; break;
    case 'N': kind = TokenType_::N; break;

    case 'X': kind = TokenType_::X; break;
    case 'Y': kind = TokenType_::Y; break;
    case 'Z': kind = TokenType_::Z; break;
    case 'R': kind = TokenType_::R; break;
    case 'F': kind = TokenType_::F; break;
    case 'O': kind = TokenType_::O; break;
    case 'S': kind = TokenType_::S; break;
    case 'H': kind = TokenType_::H; break;
    case 'P': kind = TokenType_::P; break;
    case 'I': kind = TokenType_::I; break;
    case 'J': kind = TokenType_::J; break;
    case 'E': kind = TokenType_::E; break;

    default: kind = TokenType_::Unknown;
    }

    return Token {
        pos_-1,
        1,
        kind,
    };
}

Token Lexer::tokenize_comment_(bool untilEol/* = false*/)
{
    bool done;
    unsigned start = pos_ - 1;
    wchar_t stop = untilEol ? '\n' : ')';

    while (pos_ < text_length_) {
        done = text_[pos_] == stop;
        ++pos_;
        if (done) break;
    }

    return Token {
        start,
        pos_-start,
        TokenType_::Comment,
    };
}

Token Lexer::tokenize_number_()
{
    //auto kind = TokenType_::Integer;
    unsigned start = pos_ - 1;

    scan_integer_();
    if (pos_ < text_length_ && text_[pos_] == '.') {
        ++pos_;
        if (pos_ < text_length_ && iswdigit(text_[pos_])) {
            scan_integer_();
        }
        //kind = TokenType_::Decimal;
    }

    return Token {
        start,
        pos_-start,
        TokenType_::Number,
    };
}
