#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"
#include <vector>
#include <stdexcept>
#include <memory>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <functional>
#include "tokens.h"

struct ASTNode
{
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> children;

    explicit ASTNode(const std::string& name) : name(name) {}

    void addChild(const std::shared_ptr<ASTNode>& child)
    {
        children.push_back(child);
    }
};

void generateDotFile(const std::shared_ptr<ASTNode>& root, const std::string& filename)
{
    std::ofstream out(filename);
    if (!out.is_open())
        throw std::runtime_error("Error al crear el archivo .dot");

    out << "digraph AST {\n";
    int counter = 0;

    std::function<void(const std::shared_ptr<ASTNode>&, int)> traverse;
    traverse = [&out, &counter, &traverse](const std::shared_ptr<ASTNode>& node, int parentID)
    {
        int nodeID = counter++;
        out << "  node" << nodeID << " [label=\"" << node->name << "\"];\n";

        if (parentID >= 0)
            out << "  node" << parentID << " -> node" << nodeID << ";\n";

        for (const auto& child : node->children)
            traverse(child, nodeID);
    };

    traverse(root, -1);
    out << "}\n";
}

void generatePng(const std::string& dotFilename, const std::string& pngFilename)
{
    std::string command = "dot -Tpng " + dotFilename + " -o " + pngFilename;
    int result = std::system(command.c_str());
    if (result != 0)
        throw std::runtime_error("Error al ejecutar Graphviz para generar el archivo PNG.");
}

class Parser
{
    std::vector<Token> tokens;
    size_t current = 0;

    public:
        explicit Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

        std::shared_ptr<ASTNode> parse()
        {
            auto root = std::make_shared<ASTNode>("Programa");
            programa(root);
            return root;
        }

    private:
        void programa(const std::shared_ptr<ASTNode>& parent)
        {
            auto listaSentenciasNode = std::make_shared<ASTNode>("ListaSentencias");
            parent->addChild(listaSentenciasNode);
            listaSentencias(listaSentenciasNode);
            if (!isAtEnd())
                throw std::runtime_error("Error: tokens inesperados al final del programa.");
        }

        void listaSentencias(const std::shared_ptr<ASTNode>& parent)
        {
            while (!isAtEnd())
            {
                auto sentenciaNode = std::make_shared<ASTNode>("Sentencia");
                parent->addChild(sentenciaNode);
                sentencia(sentenciaNode);
            }
        }

        void sentencia(const std::shared_ptr<ASTNode>& parent)
        {
            consume(START_SENTENCIA, "Se esperaba START_SENTENCIA.");
            if (match(CREATE))
            {
                auto createNode = std::make_shared<ASTNode>("CREATE");
                parent->addChild(createNode);
                if (check(DATABASE))
                    createDatabase(createNode);
                else if (check(TABLE))
                    createTable(createNode);
                else
                    throw std::runtime_error("Error: se esperaba DATABASE o TABLE después de CREATE.");
            }
            else
                throw std::runtime_error("Error: comando no reconocido.");
            consume(END_SENTENCIA, "Se esperaba END_SENTENCIA.");
            consume(SALTO_DE_LINEA, "Se esperaba un salto de línea.");
        }

        void createDatabase(const std::shared_ptr<ASTNode>& parent)
        {
            consume(DATABASE, "Se esperaba DATABASE.");
            auto identifierNode = std::make_shared<ASTNode>("IDENTIFICADOR: " + peek().lexeme);
            parent->addChild(identifierNode);
            consume(IDENTIFICADOR, "Se esperaba un identificador para el nombre de la base de datos.");
        }

        void createTable(const std::shared_ptr<ASTNode>& parent)
        {
            consume(TABLE, "Se esperaba TABLE.");
            auto identifierNode = std::make_shared<ASTNode>("IDENTIFICADOR: " + peek().lexeme);
            parent->addChild(identifierNode);
            consume(IDENTIFICADOR, "Se esperaba un identificador para el nombre de la tabla.");
            consume(PAREN_IZQ, "Se esperaba '('");
            auto columnsNode = std::make_shared<ASTNode>("Columnas");
            parent->addChild(columnsNode);
            listaColumnasDef(columnsNode);
            consume(PAREN_DER, "Se esperaba ')'");
        }

        void listaColumnasDef(const std::shared_ptr<ASTNode>& parent)
        {
            auto columnNode = std::make_shared<ASTNode>("Columna");
            parent->addChild(columnNode);
            columnNode->addChild(std::make_shared<ASTNode>("IDENTIFICADOR: " + peek().lexeme));
            consume(IDENTIFICADOR, "Se esperaba un identificador para el nombre de la columna.");
            tipoColumna(columnNode);

            while (match(COMA))
            {
                auto columnNodeExtra = std::make_shared<ASTNode>("Columna");
                parent->addChild(columnNodeExtra);
                columnNodeExtra->addChild(std::make_shared<ASTNode>("IDENTIFICADOR: " + peek().lexeme));
                consume(IDENTIFICADOR, "Se esperaba un identificador.");
                tipoColumna(columnNodeExtra);
            }
        }

        void tipoColumna(const std::shared_ptr<ASTNode>& parent)
        {
            if (match(INT))
                parent->addChild(std::make_shared<ASTNode>("Tipo: INT"));
            else if (match(VARCHAR))
                parent->addChild(std::make_shared<ASTNode>("Tipo: VARCHAR"));
            else if (match(BOOLEAN))
                parent->addChild(std::make_shared<ASTNode>("Tipo: BOOLEAN"));
            else
                throw std::runtime_error("Error: tipo de columna inválido.");
        }

        bool match(TokenType type)
        {
            if (check(type))
            {
                advance();
                return true;
            }
            return false;
        }

        bool check(TokenType type)
        {
            return !isAtEnd() && peek().type == type;
        }

        void consume(TokenType type, const std::string& message)
        {
            if (check(type))
                advance();
            else
                throw std::runtime_error(message);
        }

        Token advance()
        {
            if (!isAtEnd()) current++;
            return previous();
        }

        Token peek() const
        {
            return tokens[current];
        }

        Token previous() const
        {
            return tokens[current - 1];
        }

        bool isAtEnd() const
        {
            return current >= tokens.size();
        }
};

#endif
