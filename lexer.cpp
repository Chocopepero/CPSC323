#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "lexer.hpp"
enum State {
  START,
  IDENTIFIER,
  INTEGER,
  REAL,
  SEPARATOR,
  OPERAND,
  COMMENT,
  DONE
};

std::set<std::string> keywords = {"function", "integer", "boolean", "real",
                                  "if",       "else",    "endif",   "while",
                                  "endwhile", "return",  "scan",    "print"};

extern int line_number;

TokenResult lexer(std::istream &stream) {
  State state = START;
  std::string lexeme;
  char c;

  while (stream.get(c)) {
    if (c == '\n') {
      line_number++; // Increment line number on newline
    }
    switch (state) {
    case START:
      if (isalpha(c)) {
        state = IDENTIFIER;
        lexeme += c;
      } else if (isdigit(c)) {
        state = INTEGER;
        lexeme += c;
      } else if (c == '[' && stream.peek() == '*') {
        state = COMMENT;
        stream.get(); // Consume '*'
      } else if (isspace(c)) {
        continue;
      } else if (c == '(' || c == ')' || c == ';' || c == ',' || c == '[' ||
                 c == ']' || c == '{' || c == '}') {
        state = SEPARATOR;
        lexeme += c;
      } else if (c == '$' && stream.peek() == '$') {
        state = SEPARATOR;
        lexeme += c;
        stream.get(c);
        lexeme += c;
        return {"Separator", lexeme};
      } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' ||
                 c == '<' || c == '>' || c == '!') {
        state = OPERAND;
        lexeme += c;
      } else {
        state = DONE;
        return {"Invalid", std::string(1, c)};
      }
      break;

    case IDENTIFIER:
      while (isalnum(c) || c == '_') {
        lexeme += c;
        if (!stream.get(c))
          break;
      }
      stream.putback(c);
      state = DONE;
      return {keywords.count(lexeme) ? "Keyword" : "Identifier", lexeme};
      break;

    case INTEGER:
      while (isdigit(c)) {
        lexeme += c;
        if (!stream.get(c))
          break;
      }
      if (c == '.') {
        lexeme += c;
        if (!stream.get(c) || !isdigit(c)) {
          return {"Invalid", lexeme};
          state = DONE;
        } else {
          state = REAL;
          lexeme += c;
        }
      } else {
        stream.putback(c);
        state = DONE;
        return {"Integer", lexeme};
      }
      break;

    case REAL:
      while (isdigit(c)) {
        lexeme += c;
        if (!stream.get(c))
          break;
      }
      stream.putback(c);
      state = DONE;
      return {"Real", lexeme};
      break;

    case COMMENT:
      while (stream.get(c)) {
        if (c == '*' && stream.peek() == ']') {
          stream.get(); // Consume ']'
          state = START;
          break;
        }
      }
      break;

    case SEPARATOR:
      state = DONE;
      stream.putback(c);
      return {"Separator", lexeme};
      break;

    case OPERAND: {
      // For '<': form "<=" if possible.
      if (lexeme == "<") {
        if (c == '=') {
          lexeme += c;
        } else {
          stream.putback(c);
        }
      }
      // For '=': form "=>" if the next character is '>', otherwise "==" if the
      // next is '='.
      else if (lexeme == "=") {
        if (c == '>') {
          lexeme += c; // Now lexeme becomes "=>"
        } else if (c == '=') {
          lexeme += c; // Now lexeme becomes "=="
        } else {
          stream.putback(c);
        }
      }
      // For '!': form "!=" if supported.
      else if (lexeme == "!") {
        if (c == '=') {
          lexeme += c; // Now lexeme becomes "!="
        } else {
          stream.putback(c);
        }
      }
      state = DONE;
      return {"Operator", lexeme};
    }

    case DONE:
      stream.putback(c);
      lexeme.clear();
      state = START;
      break;
    }
  }
  if (stream.eof()) {
    return {"EOF", ""};
  } else {
    return {"Invalid", std::string(1, c)};
  }
}