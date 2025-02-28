#include <iostream>
#include <set>
#include <string>
#include <vector>
// Input: String of chars
// Output: Token and type.
// Identifier: l(l|d|_)*
// Integer: d+
// Real: d+.d+
// Keywords: integer, if, else, endif, while, return, scan, print etc.
// Comments: [* *]

enum Token
{
    Identifier,
    Integer,
    Real,
    Keyword,
    Invalid
};

typedef struct
{
    Token token;
    std::string lexeme;
} TokenResult;

std::set<std::string> keywords = {"integer", "if", "else", "endif", "while", "return", "scan", "print"};

std::vector<TokenResult> lexer(std::istream &stream = std::cin)
{   bool isComment = false;
    std::vector<TokenResult> tokens;
    while (!stream.eof())
    {
        char c;
        std::string lexeme;
        c = stream.get();

        if (c == EOF)
        {
           break;
        }

        if (c == '[' && stream.peek() == '*' && !isComment)
        {
            isComment = true;
            stream.get();
            continue;
        }

        if (c == '*' && stream.peek() == ']' && isComment)
        {
            isComment = false;
            stream.get();
            continue;
        }

        if (isalpha(c))
        {
            // Identifier
            while (isalnum(c) || c == '_')
            {
                lexeme += c;
                c = stream.get();
            }
            stream.putback(c);

            if(keywords.find(lexeme) != keywords.end()) {
                tokens.push_back({Keyword, lexeme});
            } else {
            tokens.push_back({Identifier, lexeme});
        }
        }

        else if (isdigit(c))
        {
            // Integer or Real
            while (isdigit(c))
            {
                lexeme += c;
                c = stream.get();
            }

            if(c == '.') {
                lexeme += c;
                c = stream.get();
                if(isdigit(c)) {
                    while(isdigit(c)) {
                        lexeme += c;
                        c = stream.get();
                    }
                    stream.putback(c);
                    tokens.push_back({Real, lexeme});
                } else {
                    tokens.push_back({Invalid, ""});
                }
            } else {
                stream.putback(c);
                tokens.push_back({Integer, lexeme});
            }
        }

        else if (c == ' ' || c == '\n')
        {
            continue;
        }
        else
        {
            tokens.push_back({Invalid, ""});
        }
    }
    return tokens;
}