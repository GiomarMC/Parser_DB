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
            while (!isAtEnd() && check(START_SENTENCIA))
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
            else if (match(SELECT))
            {
                auto selectNode = std::make_shared<ASTNode>("SELECT");
                parent->addChild(selectNode);
                select(selectNode);
            }
            else if (match(INSERT))
            {
                auto inserNode = std::make_shared<ASTNode>("INSERT");
                parent->addChild(inserNode);
                insert(inserNode);
            }
            else
                throw std::runtime_error("Error: comando no reconocido.");
            consume(END_SENTENCIA, "Se esperaba END_SENTENCIA.");
            consume(SALTO_DE_LINEA, "Se esperaba un salto de línea.");
        }

        void select(const std::shared_ptr<ASTNode>& parent)
        {
            auto proyeccionNode = std::make_shared<ASTNode>("Proyeccion");
            parent->addChild(proyeccionNode);
            if(match(ALL_COLUMNS))
                proyeccionNode->addChild(std::make_shared<ASTNode>("*"));
            else
                listaColumnas(proyeccionNode);

            consume(FROM, "Se esperaba FROM.");
            auto tableNode = std::make_shared<ASTNode>(peek().lexeme);
            parent->addChild(tableNode);
            consume(IDENTIFICADOR, "Se esperaba un identificador para la tabla.");

            auto condicionResultadoNode = std::make_shared<ASTNode>("CondicionResultado");
            parent->addChild(condicionResultadoNode);
            condicionResultado(condicionResultadoNode);
        }

        void listaColumnas(const std::shared_ptr<ASTNode>& parent)
        {
            auto columnNode = std::make_shared<ASTNode>(peek().lexeme);
            parent->addChild(columnNode);
            consume(IDENTIFICADOR, "Se esperaba un identificador en ListaColumnas.");

            listaColumnasPrime(parent);
        }

        void condicion(const std::shared_ptr<ASTNode>& parent)
        {
            consume(WHERE, "Se esperaba WHERE al inicio de la condición.");
            auto expresionNode = std::make_shared<ASTNode>("ExpresionCondicional");
            parent->addChild(expresionNode);
            expresionCondicional(expresionNode);
        }

        void condicionResultado(const std::shared_ptr<ASTNode>& parent)
        {
            if (check(WHERE)) // Si comienza con WHERE, es una condición
            {
                auto condicionNode = std::make_shared<ASTNode>("Condicion");
                parent->addChild(condicionNode);
                condicion(condicionNode);
                condicionResultadoPrime(parent); // Puede haber más resultados o terminar
            }
            else if (check(SEPARADOR)) // Si comienza con SEPARADOR, es un resultado
            {
                resultado(parent);
            }
            else
            {
                throw std::runtime_error("Error: se esperaba una condición o un resultado.");
            }
        }

        void condicionResultadoPrime(const std::shared_ptr<ASTNode>& parent)
        {
            if (match(SEPARADOR))
                resultado(parent);
            else
                parent->addChild(std::make_shared<ASTNode>("ε"));
        }

        void insert(const std::shared_ptr<ASTNode>& parent)
        {
            consume(INTO, "Se esperaba INTO.");
            auto tableNode = std::make_shared<ASTNode>(peek().lexeme);
            parent->addChild(tableNode);
            consume(IDENTIFICADOR, "Se esperaba un identificador para la tabla.");

            consume(PAREN_IZQ, "Se esperaba '(' después de la tabla.");
            auto columnsNode = std::make_shared<ASTNode>("Columnas");
            parent->addChild(columnsNode);
            listaColumnas(columnsNode);
            consume(PAREN_DER, "Se esperaba ')' después de las columnas.");

            consume(VALUES, "Se esperaba VALUES.");
            consume(PAREN_IZQ, "Se esperaba '(' después de VALUES.");
            auto valuesNode = std::make_shared<ASTNode>("Valores");
            parent->addChild(valuesNode);
            listaValores(valuesNode);
            consume(PAREN_DER, "Se esperaba ')' después de los valores.");
        }

        void resultado(const std::shared_ptr<ASTNode>& parent)
        {
            consume(SEPARADOR, "Se esperaba un separador '|'.");
            auto resultadoNode = std::make_shared<ASTNode>("RESULTADO");
            parent->addChild(resultadoNode);

            auto identifierNode = std::make_shared<ASTNode>(peek().lexeme);
            resultadoNode->addChild(identifierNode);
            consume(IDENTIFICADOR, "Se esperaba un identificador después del separador.");
        }

        void listaColumnasPrime(const std::shared_ptr<ASTNode>& parent)
        {
            if (match(COMA))
            {
                auto columnNode = std::make_shared<ASTNode>("Columna");
                parent->addChild(columnNode);
                columnNode->addChild(std::make_shared<ASTNode>(peek().lexeme));
                consume(IDENTIFICADOR, "Se esperaba un identificador.");
                listaColumnasPrime(parent);
            }
            else
            {
                parent->addChild(std::make_shared<ASTNode>("ε"));
            }
        }

        void listaValores(const std::shared_ptr<ASTNode>& parent)
        {
            auto valueNode = std::make_shared<ASTNode>("Valor");
            parent->addChild(valueNode);
            valor(valueNode);

            while (match(COMA))
            {
                auto extraValueNode = std::make_shared<ASTNode>("Valor");
                parent->addChild(extraValueNode);
                valor(extraValueNode);
            }
        }

        void valor(const std::shared_ptr<ASTNode>& parent)
        {
            if (match(NUMERO))
                parent->addChild(std::make_shared<ASTNode>(previous().lexeme));
            else if (match(STRING))
                parent->addChild(std::make_shared<ASTNode>(previous().lexeme));
            else if (match(NULL_TYPE))
                parent->addChild(std::make_shared<ASTNode>("NULL"));
            else
                throw std::runtime_error("Error: valor inválido.");
        }

        void createDatabase(const std::shared_ptr<ASTNode>& parent)
        {
            consume(DATABASE, "Se esperaba DATABASE.");
            auto databaseNode = std::make_shared<ASTNode>("DATABASE");
            parent->addChild(databaseNode);

            auto identifierNode = std::make_shared<ASTNode>(peek().lexeme);
            databaseNode->addChild(identifierNode);
            consume(IDENTIFICADOR, "Se esperaba un identificador para el nombre de la base de datos.");
        }

        void createTable(const std::shared_ptr<ASTNode>& parent)
        {
            consume(TABLE, "Se esperaba TABLE.");
            auto tableNode = std::make_shared<ASTNode>("TABLE");
            parent->addChild(tableNode);

            auto identifierNode = std::make_shared<ASTNode>(peek().lexeme);
            tableNode->addChild(identifierNode);
            consume(IDENTIFICADOR, "Se esperaba un identificador para el nombre de la tabla.");

            consume(PAREN_IZQ, "Se esperaba '('");
            auto columnsNode = std::make_shared<ASTNode>("Columnas");
            tableNode->addChild(columnsNode);
            listaColumnasDef(columnsNode);
            consume(PAREN_DER, "Se esperaba ')'");
        }

        void listaColumnasDef(const std::shared_ptr<ASTNode>& parent)
        {
            auto columnNode = std::make_shared<ASTNode>("Columna");
            parent->addChild(columnNode);
            columnNode->addChild(std::make_shared<ASTNode>(peek().lexeme));
            consume(IDENTIFICADOR, "Se esperaba un identificador para el nombre de la columna.");
            tipoColumna(columnNode);

            while (match(COMA))
            {
                auto columnNodeExtra = std::make_shared<ASTNode>("Columna");
                parent->addChild(columnNodeExtra);
                columnNodeExtra->addChild(std::make_shared<ASTNode>(peek().lexeme));
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

        void expresionCondicional(const std::shared_ptr<ASTNode>& parent)
        {
            auto termino1Node = std::make_shared<ASTNode>("Termino");
            parent->addChild(termino1Node);
            termino(termino1Node);

            auto operadorRelacionalNode = std::make_shared<ASTNode>(tokenTypeToString(peek().type));
            parent->addChild(operadorRelacionalNode);
            operadorRelacional();

            auto termino2Node = std::make_shared<ASTNode>("Termino");
            parent->addChild(termino2Node);
            termino(termino2Node);

            expresionCondicionalPrime(parent);
        }

        void expresionCondicionalPrime(const std::shared_ptr<ASTNode>& parent)
        {
            if (match(AND) || match(OR))
            {
                auto operadorLogicoNode = std::make_shared<ASTNode>(tokenTypeToString(previous().type));
                parent->addChild(operadorLogicoNode);

                auto subExpresionNode = std::make_shared<ASTNode>("ExpresionCondicional");
                parent->addChild(subExpresionNode);
                expresionCondicional(subExpresionNode);
            }
            else
            {
                parent->addChild(std::make_shared<ASTNode>("ε"));
            }
        }

        void operadorRelacional()
        {
            if (!(match(IGUAL) || match(DIFERENTE) || match(MENOR) ||
                match(MENOR_IGUAL) || match(MAYOR) || match(MAYOR_IGUAL)))
            {
                throw std::runtime_error("Se esperaba un operador relacional.");
            }
        }

        void termino(const std::shared_ptr<ASTNode>& parent)
        {
            if (match(IDENTIFICADOR))
            {
                parent->addChild(std::make_shared<ASTNode>(previous().lexeme));
            }
            else if (match(NUMERO))
            {
                parent->addChild(std::make_shared<ASTNode>(previous().lexeme));
            }
            else if (match(STRING))
            {
                parent->addChild(std::make_shared<ASTNode>(previous().lexeme));
            }
            else
            {
                throw std::runtime_error("Se esperaba un término (identificador, número o cadena).");
            }
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
                throw std::runtime_error(
                    "Error en columna " + std::to_string(peek().column) + ": " + message +
                    ". Token encontrado: " + tokenTypeToString(peek().type));
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
