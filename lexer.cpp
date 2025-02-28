#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "lexer.hpp"
enum State
{
    START,
    IDENTIFIER,
    INTEGER,
    REAL,
    SEPARATOR,
    OPERAND,
    COMMENT,
    DONE
};

// enum TokenType
// {
//     Invalid,
//     Keyword,
//     Identifier,
//     Integer,
//     Real,
//     Separator,
//     Operand
// };

// struct TokenResult
// {
//     std::string type;
//     std::string lexeme;
// };
std::set<std::string> keywords = {"integer", "if", "else", "endif", "while", "return", "scan", "print"};

std::vector<TokenResult> lexer(std::istream &stream)
{
    State state = START;
    std::vector<TokenResult> tokens;
    std::string lexeme;
    char c;

    while (stream.get(c))
    {
        switch (state)
        {
        case START:
            if (isalpha(c))
            {
                state = IDENTIFIER;
                lexeme += c;
            }
            else if (isdigit(c))
            {
                state = INTEGER;
                lexeme += c;
            }
            else if (c == '[' && stream.peek() == '*')
            {
                state = COMMENT;
                stream.get(); // Consume '*'
            }
            else if (isspace(c))
            {
                continue;
            }
            else if (c == '(' || c == ')' || c == ';')
            {
                state = SEPARATOR;
                lexeme += c;
            }
            else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>')
            {
                state = OPERAND;
                lexeme += c;
            }
            else
            {
                state = DONE;
                tokens.push_back({"Invalid", std::string(1, c)});
            }
            break;

        case IDENTIFIER:
            while (isalnum(c) || c == '_')
            {
                lexeme += c;
                if (!stream.get(c))
                    break;
            }
            stream.putback(c);
            state = DONE;
            tokens.push_back({keywords.count(lexeme) ? "Keyword" : "Identifier", lexeme});
            break;

        case INTEGER:
            while (isdigit(c))
            {
                lexeme += c;
                if (!stream.get(c))
                    break;
            }
            if (c == '.')
            {
                lexeme += c;
                if (!stream.get(c) || !isdigit(c))
                {
                    tokens.push_back({"Invalid", lexeme});
                    state = DONE;
                }
                else
                {
                    state = REAL;
                    lexeme += c;
                }
            }
            else
            {
                stream.putback(c);
                state = DONE;
                tokens.push_back({"Integer", lexeme});
            }
            break;

        case REAL:
            while (isdigit(c))
            {
                lexeme += c;
                if (!stream.get(c))
                    break;
            }
            stream.putback(c);
            state = DONE;
            tokens.push_back({"Real", lexeme});
            break;

        case COMMENT:
            while (stream.get(c))
            {
                if (c == '*' && stream.peek() == ']')
                {
                    stream.get(); // Consume ']'
                    state = START;
                    break;
                }
            }
            break;

        case SEPARATOR:
            stream.putback(c);
            state = DONE;
            tokens.push_back({"Separator", lexeme});
            break;

        case OPERAND:
            if (c == '=')
            {
                lexeme += c;
            }
            state = DONE;
            tokens.push_back({"Operand", lexeme});
            break;

        case DONE:
            stream.putback(c);
            lexeme.clear();
            state = START;
            break;
        }
    }
    return tokens;
}