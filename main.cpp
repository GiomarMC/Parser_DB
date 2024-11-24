#include "scanner.h"

int main()
{
    std::string sourceCode = "&CREATE DATABASE test#\n&SELECT * FROM table WHERE column = 10#";
    std::vector<std::string> errorList;

    Scanner scanner(sourceCode);
    std::vector<Token> tokens = scanner.scan(errorList);

    if (!errorList.empty())
    {
        for (const auto& error : errorList)
        {
            std::cerr << error << std::endl;
        }
    }

    // Mostrar los tokens
    for (const auto& token : tokens) {
        std::cout << "Tipo: " << tokenTypeToString(token.type) 
                  << ", Lexema: " << token.lexeme 
                  << ", Columna: " << token.column << std::endl;
    }

    return 0;
}