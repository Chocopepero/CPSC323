#ifndef SYNTAX_ANALYZER_HPP
#define SYNTAX_ANALYZER_HPP

#include <string>
#include "lexer.hpp"

// Function declarations for the syntax analyzer
void nextToken();
void error(const std::string &msg);
void match(TokenResult expected);

// Grammar rule functions
void Rat25S();
void OptFunctDef();
void FunctionDefinition();
void Function();
void OptParameterList();
void ParameterList();
void Parameter();
void Qualifier();
void Body();
void OptDeclarationList();
void DeclarationList();
void Declaration();
void IDs();
void StatementList();
void Statement();
void Compound();
void Assign();
void If();
void Return();
void Print();
void Scan();
void While();
void Condition();
void Relop();
void Expression();
void ExpressionPrime();
void Term();
void TermPrime();
void Factor();
void Primary();
void Empty();

#endif