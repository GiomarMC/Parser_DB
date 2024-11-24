#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <regex>
#include <cctype>
#include <unordered_map>
#include <iostream>
#include "tokens.h"

static const std::unordered_map<std::string, TokenType> keywords = {
    {"CREATE", CREATE}, {"DATABASE", DATABASE}, {"TABLE", TABLE},
    {"SELECT", SELECT}, {"FROM", FROM}, {"WHERE", WHERE},
    {"int", INT}, {"varchar", VARCHAR}, {"boolean", BOOLEAN},
    {"and", AND}, {"or", OR}
};

bool isKeyword(const std::string& lexeme)
{
    return keywords.find(lexeme) != keywords.end();
}

class Scanner
{
    public:
        Scanner(const std::string& source) : source(source), column(1), i(0) {}

        std::vector<Token> scan(std::vector<std::string>& errorList)
        {
            std::vector<Token> tokens;

            while (i < source.size())
            {
                char current = source[i];

                if (isprint(current) == false)
                {
                    errorList.push_back("Error léxico: Caracter no reconocido en la columna " + std::to_string(column));
                    i++;
                    column++;
                    continue;
                }

                if (current == '&')
                {
                    i++;
                    tokens.push_back(Token(START_SENTENCIA, "&", column));
                    column++;
                    continue;
                }

                if (current == '#')
                {
                    tokens.push_back(Token(END_SENTENCIA, "#", column));
                    column++;
                    i++;
                    continue;
                }

                if (isspace(current))
                {
                    if (current == '\n')
                        column = 1;
                    else
                        column++;
                    i++;
                    continue;
                }

                if (isalpha(current) || current == '_')
                {
                    size_t start = i;
                    while (i < source.size() && (isalnum(source[i]) || source[i] == '_'))
                    {
                        i++;
                    }
                    std::string lexeme = source.substr(start, i - start);
                    TokenType type = isKeyword(lexeme) ? keywords.at(lexeme) : IDENTIFICADOR;
                    tokens.push_back(Token(type, lexeme, column));
                    column += lexeme.size();
                    continue;
                }

                if (isdigit(current))
                {
                    size_t start = i;
                    while (i < source.size() && isdigit(source[i]))
                    {
                        i++;
                    }
                    std::string lexeme = source.substr(start, i - start);
                    tokens.push_back(Token(NUMERO, lexeme, column));
                    column += lexeme.size();
                    continue;
                }

                if (current == '"' || current == '\'')
                {
                    char quote = current;
                    size_t start = i++;
                    while (i < source.size() && source[i] != quote)
                    {
                        i++;
                    }
                    if (i == source.size()) {
                        errorList.push_back("Error léxico: Cadena no cerrada en la columna " + std::to_string(column));
                        tokens.push_back(Token(UNKNOWN, std::string(1, current), column));
                        i++;
                    }
                    else
                        i++;
                    std::string lexeme = source.substr(start, i - start);
                    tokens.push_back(Token(STRING, lexeme, column));
                    column += lexeme.size();
                    continue;
                }

                switch (current)
                {
                    case '=': tokens.push_back(Token(IGUAL, "=", column)); break;
                    case '!':
                        if (source[i + 1] == '=')
                        {
                            tokens.push_back(Token(DIFERENTE, "!=", column));
                            i++;
                        }
                        else
                            tokens.push_back(Token(UNKNOWN, "!", column));
                        break;
                    case '<':
                        if (source[i + 1] == '=')
                        {
                            tokens.push_back(Token(MENOR_IGUAL, "<=", column));
                            i++;
                        }
                        else
                            tokens.push_back(Token(MENOR, "<", column));
                        break;
                    case '>':
                        if (source[i + 1] == '=')
                        {
                            tokens.push_back(Token(MAYOR_IGUAL, ">=", column));
                            i++;
                        }
                        else
                            tokens.push_back(Token(MAYOR, ">", column));
                        break;
                    case '(': tokens.push_back(Token(PAREN_IZQ, "(", column)); break;
                    case ')': tokens.push_back(Token(PAREN_DER, ")", column)); break;
                    case ',': tokens.push_back(Token(COMA, ",", column)); break;
                    case '|': tokens.push_back(Token(SEPARADOR, "|", column)); break;
                    case '#': tokens.push_back(Token(END_SENTENCIA, "#", column)); break;
                    case '*': tokens.push_back(Token(ALL_COLUMNS, "*", column)); break;
                    default:
                        tokens.push_back(Token(UNKNOWN, std::string(1, current), column));
                        errorList.push_back("Error léxico: Símbolo desconocido en la columna " + std::to_string(column));
                }
                column++;
                i++;
            }

            return tokens;
        }

    private:
        std::string source;
        int column;
        size_t i;
};

#endif