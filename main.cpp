#include "scanner.h"
#include "parser.h"

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

    std::cout << "Analasis lexico" << std::endl;
    for (const auto& token : tokens)
    {
        std::cout << "Tipo: " << tokenTypeToString(token.type) 
                  << ", Lexema: " << token.lexeme 
                  << ", Columna: " << token.column << std::endl;
    }

    std::cout << "Analisis sintactico" << std::endl;
    try
    {
        Parser parser(tokens);
        auto astRoot = parser.parse();
        generateDotFile(astRoot, "ast.dot");
        generatePng("ast.dot", "ast.png");
        std::cout << "AST generado exitosamente en ast.png.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    

    return 0;
}