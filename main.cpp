#include "Scanner.h"
#include "Parser.h"
#include "Executor.h"
#include <iostream>

int main()
{
    std::string sourceCode = "&CREATE DATABASE test#\n"
                             "&CREATE TABLE estudiantes (id int, nombre varchar)#\n"
                             "&INSERT INTO estudiantes (id, nombre) VALUES (1, 'Juan')#\n"
                             "&INSERT INTO estudiantes (id, nombre) VALUES (2, 'Pedro')#\n"
                             "&SELECT * FROM estudiantes WHERE id <= 1 | resultado#\n";

    std::vector<std::string> errorList;

    Scanner scanner(sourceCode);
    std::vector<Token> tokens = scanner.scan(errorList);

    if (!errorList.empty())
    {
        for (const auto& error : errorList)
            std::cerr << error << std::endl;
        return 1;
    }

    try
    {
        Parser parser(tokens);
        auto astRoot = parser.parse();
        generateDotFile(astRoot, "ast.dot");
        generatePng("ast.dot", "ast.png");
        std::cout << "AST generado exitosamente en ast.png.\n";

        Executor executor("test_schema");
        executor.execute(astRoot);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}