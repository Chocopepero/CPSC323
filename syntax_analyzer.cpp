#include <iomanip>
#include <iostream>

#include "lexer.hpp"
#include "syntax_analyzer.hpp"

TokenResult currentToken;
const bool debug = true; // For debugging output
int line_number = 1;

// Get the next token from the lexer
void nextToken() { currentToken = lexer(std::cin); }

// Error handling: print an error message and exit
void error(const std::string &msg) {
  std::cerr << "Syntax error: " << msg << " @ line "<< std::to_string(line_number) << ", token: " << currentToken.lexeme
            << '\n';
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
    error("At line " + std::to_string(line_number) + " Expected " +
          expected.token +
          (expected.lexeme != "" ? (" " + expected.lexeme) : "") +
          " but found " + currentToken.token + " " + currentToken.lexeme);
  }
}

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

// R2. <Opt Function Definitions> ::= <Function Definitions> | <Empty>
void OptFunctDef() {
  if (currentToken.token == "Keyword" && currentToken.lexeme == "function") {
    FunctionDefinition();
    if (debug)
      std::cout << "<Opt Function Definitions> ::= <Function Definitions>\n";

  } else {
    if (debug) {
      std::cout << "<Opt Function Definitions> ::= <Empty>\n";
    }
  }
}

// R3. <Function Definitions> ::= <Function> | <Function> <Function Definitions>
void FunctionDefinition() {
  Function();
  if (currentToken.token == "Keyword" && currentToken.lexeme == "function") {
    FunctionDefinition();
    if (debug)
      std::cout
          << "<Function Definitions> ::= <Function> <Function Definitions>\n";
  } else {
    if (debug)
      std::cout << "<Function Definitions> ::= <Function>\n";
  }
}

// R4. <Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt
// Declaration List> <Body>
void Function() {
  // function
  match({"Keyword", "function"});

  // <Identifier>
  match({"Identifier", ""});

  // (
  match({"Separator", "("});

  // <Opt Parameter List>
  OptParameterList();

  // )
  match({"Separator", ")"});

  // <Opt Declaration List>
  OptDeclarationList();

  // <Body>
  Body();

  if (debug) {
    std::cout << "<Function> ::= function <Identifier> ( <Opt Parameter List> "
                 ") <Opt Declaration List> <Body>\n";
  }
}

// R5. <Opt Parameter List> ::= <Parameter List> | <Empty>
void OptParameterList() {
  if (currentToken.token == "Identifier") {
    ParameterList();
    if (debug)
      std::cout << "<Opt Parameter List> ::= <Parameter List>\n";
  } else {
    if (debug)
      std::cout << "<Opt Parameter List> ::= <Empty>\n";
  }
}

// R6. <Parameter List> ::= <Parameter> | <Parameter> , <Parameter List>
void ParameterList() {
  Parameter();
  if (currentToken.token == "Separator" && currentToken.lexeme == ",") {
    match({"Separator", ","});
    ParameterList();
    if (debug) {
      std::cout << "<Parameter List> ::= <Parameter> , <Parameter List>\n";
    }
  } else {
    if (debug)
      std::cout << "<Parameter List> ::= <Parameter>\n";
  }
}

// R7. <Parameter> ::= <IDs> <Qualifier>
void Parameter() {
  IDs();
  Qualifier();

  if (debug)
    std::cout << "<Parameter> ::= <IDs> <Qualifier>\n";
}

// R8. <Qualifier> ::= integer | boolean | real
void Qualifier() {
  if (currentToken.token == "Keyword" && currentToken.lexeme == "integer") {
    match({"Keyword", "integer"});
    if (debug)
      std::cout << "<Qualifier> ::= integer\n";
  } else if (currentToken.token == "Keyword" &&
             currentToken.lexeme == "boolean") {
    match({"Keyword", "boolean"});
    if (debug)
      std::cout << "<Qualifier> ::= boolean\n";
  } else if (currentToken.token == "Keyword" && currentToken.lexeme == "real") {
    match({"Keyword", "real"});
    if (debug)
      std::cout << "<Qualifier> ::= real\n";
  } else {
    error("Expected qualifier: integer, boolean, or real");
  }
}

// R9. <Body> ::= { <Statement List> }
void Body() {
  match({"Separator", "{"});
  StatementList();
  match({"Separator", "}"});

  if (debug)
    std::cout << "<Body> ::= { <Statement List> }\n";
}

// R10. <Opt Declaration List> ::= <Declaration List> | <Empty>
void OptDeclarationList() {
  if (currentToken.token == "Keyword" &&
      (currentToken.lexeme == "integer" || currentToken.lexeme == "boolean" ||
       currentToken.lexeme == "real")) {
    DeclarationList();
    if (debug)
      std::cout << "<Opt Declaration List> ::= <Declaration List>\n";
  } else {
    if (debug)
      std::cout << "<Opt Declaration List> ::= <Empty>\n";
  }
}

// R11. <Declaration List> := <Declaration> ; | <Declaration> ; <Declaration
// List>
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

// R12. <Declaration> ::= <Qualifier> <IDs>
void Declaration() {
  Qualifier();
  IDs();
  if (debug)
    std::cout << "<Declaration> ::= <Qualifier> <IDs>\n";
}

// R13. <IDs> ::= <Identifier> | <Identifier>, <IDs>
void IDs() {
  match({"Identifier", ""});

  if (currentToken.token == "Separator" && currentToken.lexeme == ",") {
    match({"Separator", ","});
    IDs();
    if (debug)
      std::cout << "<IDs> ::= <Identifier>, <IDs>\n";
  } else {
    if (debug)
      std::cout << "<IDs> ::= <Identifier>\n";
  }
}

// R14. <Statement List> ::= <Statement> | <Statement> <Statement List>
void StatementList() {
  Statement();

  if (currentToken.token != "Separator" ||
      (currentToken.token == "Separator" && currentToken.lexeme != "}" &&
       currentToken.lexeme != "$$")) {
    StatementList();
    if (debug)
      std::cout << "<Statement List> ::= <Statement> <Statement List>\n";
  } else {
    if (debug)
      std::cout << "<Statement List> ::= <Statement>\n";
  }
}

// R15. <Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> |
// <Scan> | <While>
void Statement() {
  if (currentToken.token == "Separator" && currentToken.lexeme == "{") {
    Compound();
    if (debug)
      std::cout << "<Statement> ::= <Compound>\n";
  } else if (currentToken.token == "Identifier") {
    Assign();
    if (debug)
      std::cout << "<Statement> ::= <Assign>\n";
  } else if (currentToken.token == "Keyword") {
    if (currentToken.lexeme == "if") {
      If();
      if (debug)
        std::cout << "<Statement> ::= <If>\n";
    } else if (currentToken.lexeme == "return") {
      Return();
      if (debug)
        std::cout << "<Statement> ::= <Return>\n";
    } else if (currentToken.lexeme == "print") {
      Print();
      if (debug)
        std::cout << "<Statement> ::= <Print>\n";
    } else if (currentToken.lexeme == "scan") {
      Scan();
      if (debug)
        std::cout << "<Statement> ::= <Scan>\n";
    } else if (currentToken.lexeme == "while") {
      While();
      if (debug)
        std::cout << "<Statement> ::= <While>\n";
    } else {
      error("Invalid keyword for statement");
    }
  } else {
    error("Invalid statement");
  }
}

// R16. <Compound> ::= { <Statement List> }
void Compound() {
  match({"Separator", "{"});
  StatementList();
  match({"Separator", "}"});
  if (debug)
    std::cout << "<Compound> ::= { <Statement List> }\n";
}

// R17. <Assign> ::= <Identifier> = <Expression> ;
void Assign() {
  match({"Identifier", ""});
  match({"Operator", "="});
  Expression();
  match({"Separator", ";"});
  if (debug)
    std::cout << "<Assign> ::= <Identifier> = <Expression> ;\n";
}

// R18. <If> ::= if ( <Condition> ) <Statement> endif | if ( <Condition> )
// <Statement> else <Statement> endif
void If() {
  match({"Keyword", "if"});
  match({"Separator", "("});
  Condition();
  match({"Separator", ")"});
  Statement();

  if (currentToken.token == "Keyword" && currentToken.lexeme == "else") {
    match({"Keyword", "else"});
    Statement();
    match({"Keyword", "endif"});

    if (debug)
      std::cout
          << "<If> ::= if ( <Condition> ) <Statement> else <Statement> endif\n";
  } else {
    match({"Keyword", "endif"});

    if (debug)
      std::cout << "<If> ::= if ( <Condition> ) <Statement> endif\n";
  }
}

// R19. <Return> ::= return ; | return <Expression> ;
void Return() {
  match({"Keyword", "return"});

  if (currentToken.token == "Separator" && currentToken.lexeme == ";") {
    match({"Separator", ";"});

    if (debug)
      std::cout << "<Return> ::= return ;\n";
  } else {
    Expression();
    match({"Separator", ";"});

    if (debug)
      std::cout << "<Return> ::= return <Expression> ;\n";
  }
}

// R20. <Print> ::= print ( <Expression> );
void Print() {

  match({"Keyword", "print"});
  match({"Separator", "("});
  Expression();
  match({"Separator", ")"});
  match({"Separator", ";"});
  if (debug)
    std::cout << "<Print> ::= print ( <Expression> );\n";
}

// R21. <Scan> ::= scan ( <IDs> );
void Scan() {
  match({"Keyword", "scan"});
  match({"Separator", "("});
  IDs();
  match({"Separator", ")"});
  match({"Separator", ";"});
  if (debug)
    std::cout << "<Scan> ::= scan ( <IDs> );\n";
}

// R22. <While> ::= while ( <Condition> ) <Statement> endwhile
void While() {
  match({"Keyword", "while"});
  match({"Separator", "("});
  Condition();
  match({"Separator", ")"});
  Statement();
  match({"Keyword", "endwhile"});
  if (debug)
    std::cout << "<While> ::= while ( <Condition> ) <Statement> endwhile\n";
}

// R23. <Condition> ::= <Expression> <Relop> <Expression>
void Condition() {
  Expression();
  Relop();
  Expression();
  if (debug)
    std::cout << "<Condition> ::= <Expression> <Relop> <Expression>\n";
}

// R24. <Relop> ::= == | != | > | < | <= | =>
void Relop() {
  if (currentToken.token == "Operator") {
    if (currentToken.lexeme == "==") {
      match({"Operator", "=="});
      if (debug)
        std::cout << "<Relop> ::= ==\n";
    } else if (currentToken.lexeme == "!=") {
      match({"Operator", "!="});
      if (debug)
        std::cout << "<Relop> ::= !=\n";
    } else if (currentToken.lexeme == ">") {
      match({"Operator", ">"});
      if (debug)
        std::cout << "<Relop> ::= >\n";
    } else if (currentToken.lexeme == "<") {
      match({"Operator", "<"});
      if (debug)
        std::cout << "<Relop> ::= <\n";
    } else if (currentToken.lexeme == "<=") {
      match({"Operator", "<="});
      if (debug)
        std::cout << "<Relop> ::= <=\n";
    } else if (currentToken.lexeme == "=>") {
      match({"Operator", "=>"});
      if (debug)
        std::cout << "<Relop> ::= =>\n";
    } else {
      error("Invalid relational operator");
    }
  } else {
    error("Expected relational operator");
  }
}

// R25. <Expression> ::= <Term> <Expression'>
void Expression() {
  Term();
  ExpressionPrime();
  if (debug)
    std::cout << "<Expression> ::= <Term> <Expression'>\n";
}

// <Expression'> ::= + <Term> <Expression'> | - <Term> <Expression'> | epsilon
void ExpressionPrime() {
  if (currentToken.token == "Operator" && currentToken.lexeme == "+") {
    match({"Operator", "+"});
    Term();
    ExpressionPrime();
    if (debug)
      std::cout << "<Expression'> ::= + <Term> <Expression'>\n";
  } else if (currentToken.token == "Operator" && currentToken.lexeme == "-") {
    match({"Operator", "-"});
    Term();
    ExpressionPrime();
    if (debug)
      std::cout << "<Expression'> ::= - <Term> <Expression'>\n";
  } else {
    if (debug)
      std::cout << "<Expression'> ::= ε\n";
  }
}

// R26. <Term> ::= <Factor> <Term'>
void Term() {
  Factor();
  TermPrime();
  if (debug)
    std::cout << "<Term> ::= <Factor> <Term'>\n";
}

// <Term'> ::= * <Factor> <Term'> | / <Factor> <Term'> | epsilon
void TermPrime() {
  if (currentToken.token == "Operator" && currentToken.lexeme == "*") {
    match({"Operator", "*"});
    Factor();
    TermPrime();
    if (debug)
      std::cout << "<Term'> ::= * <Factor> <Term'>\n";
  } else if (currentToken.token == "Operator" && currentToken.lexeme == "/") {
    match({"Operator", "/"});
    Factor();
    TermPrime();
    if (debug)
      std::cout << "<Term'> ::= / <Factor> <Term'>\n";
  } else {
    // Epsilon production
    if (debug)
      std::cout << "<Term'> ::= ε\n";
  }
}

// R27. <Factor> ::= - <Primary> | <Primary>
void Factor() {
  if (currentToken.token == "Operator" && currentToken.lexeme == "-") {
    match({"Operator", "-"});
    Primary();
    if (debug)
      std::cout << "<Factor> ::= - <Primary>\n";
  } else {
    if (debug)
      std::cout << "<Factor> ::= <Primary>\n";
    Primary();
  }
}

// R28. <Primary> ::= <Identifier> | <Integer> | <Identifier> ( <IDs> ) | (
// <Expression> ) | <Real> | true | false
void Primary() {
  if (currentToken.token == "Identifier") {
    match({"Identifier", ""});

    if (currentToken.token == "Separator" && currentToken.lexeme == "(") {
      match({"Separator", "("});
      IDs();
      match({"Separator", ")"});
      if (debug)
        std::cout << "<Primary> ::= <Identifier> ( <IDs> )\n";

    } else {
      if (debug)
        std::cout << "<Primary> ::= <Identifier>\n";
    }
  } else if (currentToken.token == "Integer") {
    match({"Integer", ""});

    if (debug)
      std::cout << "<Primary> ::= <Integer>\n";
  } else if (currentToken.token == "Real") {
    match({"Real", ""});

    if (debug)
      std::cout << "<Primary> ::= <Real>\n";
  } else if (currentToken.token == "Separator" && currentToken.lexeme == "(") {
    match({"Separator", "("});
    Expression();
    match({"Separator", ")"});
    if (debug)
      std::cout << "<Primary> ::= ( <Expression> )\n";

  } else if (currentToken.token == "Keyword" && currentToken.lexeme == "true") {
    match({"Keyword", "true"});

    if (debug)
      std::cout << "<Primary> ::= true\n";
  } else if (currentToken.token == "Keyword" &&
             currentToken.lexeme == "false") {
    match({"Keyword", "false"});

    if (debug)
      std::cout << "<Primary> ::= false\n";
  } else {
    error("Expected primary expression");
  }
}

// R29. <Empty> ::=
void Empty() {
  if (debug)
    std::cout << "<Empty> ::= ε\n";
}