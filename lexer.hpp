#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <set>

typedef struct
{
    std::string token;
    std::string lexeme;
} TokenResult;

TokenResult lexer(std::istream &stream = std::cin);

#endif // LEXER_H