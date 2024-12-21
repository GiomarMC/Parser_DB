#include "Scanner.h"
#include <cctype>
#include <iostream>
#include <regex>

// Definición del mapa de palabras clave
const std::unordered_map<std::string, TokenType> keywords = {
    {"CREATE", CREATE}, {"DATABASE", DATABASE}, {"TABLE", TABLE},
    {"SELECT", SELECT}, {"INSERT", INSERT}, {"INTO", INTO}, {"VALUES", VALUES},
    {"FROM", FROM}, {"WHERE", WHERE}, {"int", INT}, {"varchar", VARCHAR},
    {"boolean", BOOLEAN}, {"and", AND}, {"or", OR}, {"\n", SALTO_DE_LINEA}
};

// Verifica si un lexema es una palabra clave
bool isKeyword(const std::string& lexeme)
{
    return keywords.find(lexeme) != keywords.end();
}

// Constructor del Scanner
Scanner::Scanner(const std::string& source)
    : source(source), column(1), i(0)
{}

// Método para escanear tokens
std::vector<Token> Scanner::scan(std::vector<std::string>& errorList)
{
    std::vector<Token> tokens;

    while (i < source.size())
    {
        char current = source[i];

        // Ignorar caracteres no imprimibles
        if (!isprint(current) && current != '\n')
        {
            errorList.push_back("Error léxico: Caracter no reconocido en la columna " + std::to_string(column));
            i++;
            column++;
            continue;
        }

        // Manejar símbolos especiales
        if (current == '&')
        {
            tokens.emplace_back(START_SENTENCIA, "&", column);
            i++;
            column++;
            continue;
        }

        if (current == '#')
        {
            tokens.emplace_back(END_SENTENCIA, "#", column);
            i++;
            column++;
            continue;
        }

        // Manejar espacios en blanco y saltos de línea
        if (isspace(current))
        {
            if (current == '\n')
            {
                tokens.emplace_back(SALTO_DE_LINEA, "\\n", column);
                column = 1;
            }
            else
            {
                column++;
            }
            i++;
            continue;
        }

        // Manejar identificadores y palabras clave
        if (isalpha(current) || current == '_')
        {
            size_t start = i;
            while (i < source.size() && (isalnum(source[i]) || source[i] == '_'))
            {
                i++;
            }
            std::string lexeme = source.substr(start, i - start);
            TokenType type = isKeyword(lexeme) ? keywords.at(lexeme) : IDENTIFICADOR;
            tokens.emplace_back(type, lexeme, column);
            column += lexeme.size();
            continue;
        }

        // Manejar números
        if (isdigit(current))
        {
            size_t start = i;
            while (i < source.size() && isdigit(source[i]))
            {
                i++;
            }
            std::string lexeme = source.substr(start, i - start);
            tokens.emplace_back(NUMERO, lexeme, column);
            column += lexeme.size();
            continue;
        }

        // Manejar cadenas entre comillas
        if (current == '"' || current == '\'')
        {
            char quote = current;
            size_t start = i++;
            while (i < source.size() && source[i] != quote)
            {
                i++;
            }

            if (i == source.size())
            {
                errorList.push_back("Error léxico: Cadena no cerrada en la columna " + std::to_string(column));
                tokens.emplace_back(UNKNOWN, std::string(1, current), column);
                i++;
            }
            else
            {
                i++;
                std::string lexeme = source.substr(start, i - start);
                tokens.emplace_back(STRING, lexeme, column);
                column += lexeme.size();
            }
            continue;
        }

        // Manejar operadores y otros símbolos
        switch (current)
        {
            case '=': tokens.emplace_back(IGUAL, "=", column); break;
            case '!':
                if (source[i + 1] == '=')
                {
                    tokens.emplace_back(DIFERENTE, "!=", column);
                    i++;
                }
                else
                {
                    tokens.emplace_back(UNKNOWN, "!", column);
                }
                break;
            case '<':
                if (source[i + 1] == '=')
                {
                    tokens.emplace_back(MENOR_IGUAL, "<=", column);
                    i++;
                }
                else
                {
                    tokens.emplace_back(MENOR, "<", column);
                }
                break;
            case '>':
                if (source[i + 1] == '=')
                {
                    tokens.emplace_back(MAYOR_IGUAL, ">=", column);
                    i++;
                }
                else
                {
                    tokens.emplace_back(MAYOR, ">", column);
                }
                break;
            case '(':
                tokens.emplace_back(PAREN_IZQ, "(", column);
                break;
            case ')':
                tokens.emplace_back(PAREN_DER, ")", column);
                break;
            case ',':
                tokens.emplace_back(COMA, ",", column);
                break;
            case '|':
                tokens.emplace_back(SEPARADOR, "|", column);
                break;
            case '*':
                tokens.emplace_back(ALL_COLUMNS, "*", column);
                break;
            default:
                tokens.emplace_back(UNKNOWN, std::string(1, current), column);
                errorList.push_back("Error léxico: Símbolo desconocido en la columna " + std::to_string(column));
        }
        column++;
        i++;
    }

    return tokens;
}