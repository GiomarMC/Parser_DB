#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "tokens.h"

// Mapa de palabras clave SQL
extern const std::unordered_map<std::string, TokenType> keywords;

// Función para verificar si un lexema es palabra clave
bool isKeyword(const std::string& lexeme);

// Clase Scanner
class Scanner
{
public:
    // Constructor
    Scanner(const std::string& source);

    // Método principal para escanear el código fuente
    std::vector<Token> scan(std::vector<std::string>& errorList);

private:
    std::string source;
    int column;
    size_t i;
};

#endif // SCANNER_H