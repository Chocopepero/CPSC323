#include "syntax_analyzer.hpp"
#include "lexer.hpp"
#include <iostream>
#include <string>

TokenResult currentToken;
const bool debug = true; // For debugging output
int line_number = 1;

// Get the next token from the lexer
void nextToken() { 
  currentToken = lexer(std::cin);
  if (currentToken.token == "Separator" && currentToken.lexeme == "\n") {
    line_number++;
  }
}

// Error handling: print an error message and exit
void error(const std::string &msg) {
  std::cerr << "Syntax error: " << msg << ", token: " << currentToken.lexeme
            << std::endl;
  exit(1);
}

// Match the expected token and advance the token stream
void match(TokenResult expected) {
    if (currentToken.token == expected.token &&
        (expected.lexeme == "" || currentToken.lexeme == expected.lexeme)) {
      std::cout << "Token: " << currentToken.token
                << "\tLexeme: " << currentToken.lexeme << '\n';
      nextToken();
    } else {
      error("At line " + std::to_string(line_number) + " Expected " + expected.token + (expected.lexeme != "" ? (" " + expected.lexeme) : "")
            + " but found " + currentToken.token + " " + currentToken.lexeme);
    }
  }
  

// Grammar rule functions
void Rat25S() {
  match({"Separator", "$$"});
  OptFunctDef();
  match({"Separator", "$$"});
  OptDeclarationList();
  match({"Separator", "$$"});
  StatementList();
  match({"Separator", "$$"});

  if (currentToken.token != "EOF") {
    error("Expected EOF");
  }
  if (debug)
    std::cout << "<Rat25S> ::= $$ <Opt Function Definitions> $$ <Opt "
                 "Declaration List> $$ <Statement List> $$\n";
}

void OptFunctDef() {
  if (currentToken.token == "Keyword" && currentToken.lexeme == "function") {
    if (debug)
      std::cout << "<Opt Function Definitions> -> <Function Definition>\n";
    FunctionDefinition();
  } else {
    if (debug)
      std::cout << "<Opt Function Definitions> -> ε\n";
  }
}

void FunctionDefinition() {
  Function();
  if (currentToken.token == "Keyword" && currentToken.lexeme == "function") {
    FunctionDefinition();
    if (debug)
      std::cout
          << "<Function Definition> ::= <Function> <Function Definition>\n";
  } else {
    if (debug)
      std::cout << "<Function Definition> ::= <Function>\n";
  }
}

void Function() {
  match({"Keyword", "function"});
  match({"Identifier", ""});
  match({"Separator", "("});
  OptParameterList();
  match({"Separator", ")"});
  OptDeclarationList();
  Body();
  if (debug)
    std::cout << "<Function> ::= function <Identifier> ( <Opt Parameter List> "
                 ") <Opt Declaration List> <Body>\n";
}

void OptParameterList() {
  if (currentToken.token == "Identifier") {
    ParameterList();
  } else {
    if (debug)
      std::cout << "<Opt Parameter List> -> ε\n";
  }
}

void ParameterList() {
  Parameter();
  if (currentToken.token == "Separator" && currentToken.lexeme == ",") {
    match({"Separator", ","});
    ParameterList();
    if (debug)
      std::cout << "<Parameter List> ::= <Parameter> , <Parameter List>\n";
  } else {
    if (debug)
      std::cout << "<Parameter List> ::= <Parameter>\n";
  }
}

void Parameter() {
  IDs();
  Qualifier();
  if (debug)
    std::cout << "<Parameter> ::= <IDs> <Qualifier>\n";
}

void Qualifier() {
    if (currentToken.lexeme == "integer" || currentToken.lexeme == "boolean" ||
        currentToken.lexeme == "real") {
      std::string qual = currentToken.lexeme;
      match({"Keyword", qual});
      if (debug)
        std::cout << "<Qualifier> ::= " << qual << '\n';
    } else {
      error("Expected qualifier: integer, boolean, or real");
    }
  }
  

void Body() {
  match({"Separator", "{"});
  StatementList();
  match({"Separator", "}"});
  if (debug)
    std::cout << "<Body> ::= { <Statement List> }\n";
}

void OptDeclarationList() {
  if (currentToken.token == "Keyword" &&
      (currentToken.lexeme == "integer" || currentToken.lexeme == "boolean" ||
       currentToken.lexeme == "real")) {
    DeclarationList();
    if (debug)
      std::cout << "<Opt Declaration List> -> <Declaration List>\n";
  } else {
    if (debug)
      std::cout << "<Opt Declaration List> -> ε\n";
  }
}

void DeclarationList() {
  Declaration();
  match({"Separator", ";"});
  if (currentToken.token == "Keyword" &&
      (currentToken.lexeme == "integer" || currentToken.lexeme == "boolean" ||
       currentToken.lexeme == "real")) {
    DeclarationList();
    if (debug)
      std::cout
          << "<Declaration List> ::= <Declaration> ; <Declaration List>\n";
  } else {
    if (debug)
      std::cout << "<Declaration List> ::= <Declaration> ;\n";
  }
}

void Declaration() {
  Qualifier();
  IDs();
  if (debug)
    std::cout << "<Declaration> ::= <Qualifier> <IDs>\n";
}

void IDs() {
  match({"Identifier", ""});
  if (currentToken.token == "Separator" && currentToken.lexeme == ",") {
    match({"Separator", ","});
    IDs();
    if (debug)
      std::cout << "<IDs> ::= <Identifier> , <IDs>\n";
  } else {
    if (debug)
      std::cout << "<IDs> ::= <Identifier>\n";
  }
}

void StatementList() {
  Statement();
  if (currentToken.token == "Identifier" || currentToken.token == "Keyword") {
    StatementList();
    if (debug)
      std::cout << "<Statement List> ::= <Statement> <Statement List>\n";
  } else {
    if (debug)
      std::cout << "<Statement List> ::= <Statement>\n";
  }
}

void Statement() {
  if (currentToken.token == "Separator" && currentToken.lexeme == "{") {
    Compound();
    if (debug)
      std::cout << "<Statement> ::= <Compound>\n";
  } else if (currentToken.token == "Identifier") {
    Assign();
    if (debug)
      std::cout << "<Statement> ::= <Assign>\n";
  } else if (currentToken.token == "Keyword" && currentToken.lexeme == "if") {
    If();
    if (debug)
      std::cout << "<Statement> ::= <If>\n";
  } else if (currentToken.token == "Keyword" &&
             currentToken.lexeme == "return") {
    Return();
    if (debug)
      std::cout << "<Statement> ::= <Return>\n";
  } else if (currentToken.token == "Keyword" &&
             currentToken.lexeme == "print") {
    Print();
    if (debug)
      std::cout << "<Statement> ::= <Print>\n";
  } else if (currentToken.token == "Keyword" && currentToken.lexeme == "scan") {
    Scan();
    if (debug)
      std::cout << "<Statement> ::= <Scan>\n";
  } else if (currentToken.token == "Keyword" &&
             currentToken.lexeme == "while") {
    WhileStatement();
    if (debug)
      std::cout << "<Statement> ::= <While Statement>\n";
  } else {
    error("Invalid statement");
  }
}

void Compound() {
  match({"Separator", "{"});
  StatementList();
  match({"Separator", "}"});
  if (debug)
    std::cout << "<Compound> ::= { <Statement List> }\n";
}

void Assign() {
  match({"Identifier", ""});
  match({"Operator", "="});
  Expression();
  match({"Separator", ";"});
  if (debug)
    std::cout << "<Assign> ::= <Identifier> = <Expression> ;\n";
}

void If() {
  match({"Keyword", "if"});
  match({"Separator", "("});
  Condition();
  match({"Separator", ")"});
  Statement();
  if (currentToken.token == "Keyword" && currentToken.lexeme == "else") {
    match({"Keyword", "else"});
    Statement();
    if (debug)
      std::cout << "<If> ::= if ( <Condition> ) <Statement> else <Statement>\n";
  }
  match({"Keyword", "endif"});
  if (debug)
    std::cout << "<If> ::= if ( <Condition> ) <Statement> endif\n";
}

void Return() {
  match({"Keyword", "return"});
  if (currentToken.token != "Separator" || currentToken.lexeme != ";") {
    Expression();
    if (debug)
      std::cout << "<Return> ::= return <Expression> ;\n";
  }
  match({"Separator", ";"});
  if (debug)
    std::cout << "<Return> ::= return ;\n";
}

void Print() {
  match({"Keyword", "print"});
  match({"Separator", "("});
  Expression();
  match({"Separator", ")"});
  match({"Separator", ";"});
  if (debug)
    std::cout << "<Print> ::= print ( <Expression> ) ;\n";
}

void Scan() {
  match({"Keyword", "scan"});
  match({"Separator", "("});
  IDs();
  match({"Separator", ")"});
  match({"Separator", ";"});
  if (debug)
    std::cout << "<Scan> ::= scan ( <IDs> ) ;\n";
}

void WhileStatement() {
  match({"Keyword", "while"});
  match({"Separator", "("});
  Condition();
  match({"Separator", ")"});
  Statement();
  match({"Keyword", "endwhile"});
  if (debug)
    std::cout << "<While Statement> ::= while ( <Condition> ) "
                 "<Statement> endwhile\n";
}

void Condition() {
  Expression();
  Relop();
  Expression();
  if (debug)
    std::cout << "<Condition> ::= <Expression> <Relop> <Expression>\n";
}

void Relop() {
  auto temp = currentToken;
  if (currentToken.token == "Operator" &&
      (currentToken.lexeme == "==" || currentToken.lexeme == "!=" ||
       currentToken.lexeme == ">" || currentToken.lexeme == "<" ||
       currentToken.lexeme == "=>" || currentToken.lexeme == "<=")) {
    match({"Operator", currentToken.lexeme});
    if (debug)
      std::cout << "<Relop> ::= " << temp.lexeme << '\n';
  } else {
    error("Expected relational operator");
  }
}

void Expression() {
  Term();
  ExpressionPrime();
  if (debug)
    std::cout << "<Expression> ::= <Term> <Expression Prime>\n";
}

void ExpressionPrime() {
  if (currentToken.token == "Operator" &&
      (currentToken.lexeme == "+" || currentToken.lexeme == "-")) {
    match({"Operator", currentToken.lexeme});
    Term();
    ExpressionPrime();
    if (debug)
      std::cout << "<Expression Prime> ::= <Operator> <Term> "
                   "<Expression Prime>\n";
  }
}

void Term() {
  Factor();
  TermPrime();
  if (debug)
    std::cout << "<Term> ::= <Factor> <Term Prime>\n";
}

void TermPrime() {
  if (currentToken.token == "Operator" &&
      (currentToken.lexeme == "*" || currentToken.lexeme == "/")) {
    match({"Operator", currentToken.lexeme});
    Factor();
    TermPrime();
    if (debug)
      std::cout << "<Term Prime> ::= <Operator> <Factor> <Term Prime>\n";
  }
}

void Factor() {
  if (currentToken.token == "Operator" && currentToken.lexeme == "-") {
    match({"Operator", "-"});
    Primary();
    if (debug)
      std::cout << "<Factor> ::= - <Primary>\n";
  } else {
    Primary();
    if (debug)
      std::cout << "<Factor> ::= <Primary>\n";
  }
}

void Primary() {
  if (currentToken.token == "Identifier") {
    match({"Identifier", ""});
    if (debug)
      std::cout << "<Primary> ::= <Identifier>\n";
    if (currentToken.token == "Separator" && currentToken.lexeme == "(") {
      match({"Separator", "("});
      IDs();
      match({"Separator", ")"});
      if (debug)
        std::cout << "<Primary> ::= <Identifier> ( <IDs> )\n";
    }
  } else if (currentToken.token == "Integer" || currentToken.token == "Real") {
    match({currentToken.token, ""});
    if (debug)
      std::cout << "<Primary> ::= <" << currentToken.token << ">\n";
  } else if (currentToken.token == "Separator" && currentToken.lexeme == "(") {
    match({"Separator", "("});
    Expression();
    match({"Separator", ")"});
    if (debug)
      std::cout << "<Primary> ::= ( <Expression> )\n";
  } else if (currentToken.token == "Keyword" &&
             (currentToken.lexeme == "true" ||
              currentToken.lexeme == "false")) {
    match({"Keyword", currentToken.lexeme});
    if (debug)
      std::cout << "<Primary> ::= <Keyword> " << currentToken.lexeme << '\n';
  } else {
    error("Expected primary expression");
  }
}

void Empty() {
  if (debug)
    std::cout << "<Empty> ::= ε\n";
}
