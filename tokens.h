#ifndef TOKENS_H
#define TOKENS_H

#include <string>

enum TokenType
{
    // Palabras clave
    CREATE, DATABASE, TABLE, SELECT, FROM, WHERE,
    INT, VARCHAR, BOOLEAN,

    // Identificadores
    IDENTIFICADOR,

    // Literales
    NUMERO,                         // Numeros enteros
    STRING,                         // Cadenas de texto

    // Operadores
    IGUAL,                         // =
    DIFERENTE,                     // !=
    MENOR,                         // <
    MENOR_IGUAL,                   // <=
    MAYOR,                         // >
    MAYOR_IGUAL,                   // >=
    AND,                           // and
    OR,                            // or

    // Simbolos especiales
    PAREN_IZQ,                     // (
    PAREN_DER,                     // )
    COMA,                          // ,
    SEPARADOR,                     // |

    // Otros
    START_SENTENCIA,                // &
    END_SENTENCIA,                 // #
    ALL_COLUMNS,                   // *
    UNKNOWN,                       // Token desconocido
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int column;

    Token(TokenType t, const std::string& l, int col): type(t), lexeme(l), column(col) {}
};

std::string tokenTypeToString(TokenType type)
{
    switch(type)
    {
        case START_SENTENCIA: return "START_SENTENCIA";
        case CREATE: return "CREATE";
        case DATABASE: return "DATABASE";
        case TABLE: return "TABLE";
        case SELECT: return "SELECT";
        case FROM: return "FROM";
        case WHERE: return "WHERE";
        case INT: return "INT";
        case VARCHAR: return "VARCHAR";
        case BOOLEAN: return "BOOLEAN";
        case AND: return "AND";
        case OR: return "OR";
        case IDENTIFICADOR: return "IDENTIFICADOR";
        case NUMERO: return "NUMERO";
        case STRING: return "STRING";
        case IGUAL: return "IGUAL";
        case DIFERENTE: return "DIFERENTE";
        case MENOR: return "MENOR";
        case MENOR_IGUAL: return "MENOR_IGUAL";
        case MAYOR: return "MAYOR";
        case MAYOR_IGUAL: return "MAYOR_IGUAL";
        case PAREN_IZQ: return "PAREN_IZQ";
        case PAREN_DER: return "PAREN_DER";
        case COMA: return "COMA";
        case SEPARADOR: return "SEPARADOR";
        case END_SENTENCIA: return "END_SENTENCIA";
        case ALL_COLUMNS: return "ALL_COLUMNS";
        case UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

#endif