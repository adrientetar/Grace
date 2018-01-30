/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#include <wx/string.h>

namespace Token {
    enum Type {
        N,
        O,

        G,

        X,
        Y,
        Z,
        U,
        V,
        W,
        P,
        Q,
        R,
        A,
        B,
        C,

        I,
        J,
        K,

        E,
        F,

        S,

        D,
        T,

        M,

        Comment,
        Number,

        AlignmentChar,
        Equal,
        OptBlockSkip,
        Percent,

        EndOfBlock,
        EndOfFile,
        Unknown
    };
    struct Token {
        unsigned start;
        unsigned length;
        Type type;
    };
};

inline std::string TokenType_ToString(Token::Type tt)
{
    switch (tt)
    {
        case Token::N:   return "N";
        case Token::O:   return "O";

        case Token::G:   return "G";

        case Token::X:   return "X";
        case Token::Y:   return "Y";
        case Token::Z:   return "Z";
        case Token::U:   return "U";
        case Token::V:   return "V";
        case Token::W:   return "W";
        case Token::P:   return "P";
        case Token::Q:   return "Q";
        case Token::R:   return "R";
        case Token::A:   return "A";
        case Token::B:   return "B";
        case Token::C:   return "C";

        case Token::I:   return "I";
        case Token::J:   return "J";
        case Token::K:   return "K";

        case Token::E:   return "E";
        case Token::F:   return "F";

        case Token::S:   return "S";

        case Token::D:   return "D";
        case Token::T:   return "T";

        case Token::M:   return "M";

        case Token::Comment:   return "Comment";
        case Token::Number:   return "Number";

        case Token::AlignmentChar:   return ":";
        case Token::Equal:   return "=";
        case Token::OptBlockSkip:   return "/";
        case Token::Percent:   return "%";

        case Token::EndOfBlock:   return "EndOfBlock";
        case Token::EndOfFile:   return "EndOfFile";
        case Token::Unknown:   return "Unknown";

        default:   return "<invalid>";
    }
}

inline std::ostream& operator<<(std::ostream& stream,
                         const Token::Token& t) {
    return stream << TokenType_ToString(t.type) << "(" << t.start << "," << t.length << ")" << std::endl;
}

/* */

class Word {
public:
    Word(Token::Type kind, float value) : kind(kind), value{value} {}
    bool operator==(const Word& rhs) const {
        return (kind == rhs.kind) &&
               (value == rhs.value);
    }
    Token::Type kind;
    float value;
};

inline std::string Word_ToString(Word& w)
{
    std::string value;
    if (std::trunc(w.value) == w.value)
    {
        value = std::to_string(int(w.value));
    }
    else
    {
        value = std::to_string(w.value);
    }
    return TokenType_ToString(w.kind) + value;
}

namespace std
{
    template <>
    struct hash<Word>
    {
        size_t operator()(const Word& w) const
        {
            return ((hash<int>()(static_cast<int>(w.kind))
                    ^ (hash<float>()(w.value) << 1)) >> 1);
        }
    };
}

using TokenSet = std::unordered_set<Token::Type>;
using WordSet = std::unordered_set<Word>;

class Header {
public:
    Header() { }
    std::optional<std::variant<unsigned, std::wstring>> identifier;
};

class BlockNumber {
public:
    BlockNumber(unsigned value) : value{value} { }
    unsigned value;
};

class Block {
public:
    Block() { }
    std::optional<BlockNumber> number;
    std::vector<Word> data_words;
};

class Program {
public:
    Program() { }
    Header header;
    std::vector<Block> blocks;
};

/* */

class PosException : public std::exception
{
public:
    PosException(std::string msg, unsigned position, unsigned length)
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
