#include <iostream>
#include <iomanip>
#include <vector>

#include "lexer.hpp"

int main()
{
    std::vector<TokenResult> tokens = lexer();
    for (auto token : tokens)
    {
        std::cout << std::left << "Token: " << std::setw(20) << token.token
                  << std::setw(10) << "Lexeme: " << token.lexeme << '\n';
    }
    return 0;
}