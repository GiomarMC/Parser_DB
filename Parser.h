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

extern void generateDotFile(const std::shared_ptr<ASTNode>& root, const std::string& filename);
extern void generatePng(const std::string& dotFilename, const std::string& pngFilename);

class Parser
{
    std::vector<Token> tokens;
    size_t current = 0;

    public:
        explicit Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

        std::shared_ptr<ASTNode> parse();

    private:
        void programa(const std::shared_ptr<ASTNode>& parent);

        void listaSentencias(const std::shared_ptr<ASTNode>& parent);

        void sentencia(const std::shared_ptr<ASTNode>& parent);

        void select(const std::shared_ptr<ASTNode>& parent);

        void listaColumnas(const std::shared_ptr<ASTNode>& parent);

        void condicion(const std::shared_ptr<ASTNode>& parent);

        void condicionResultado(const std::shared_ptr<ASTNode>& parent);

        void condicionResultadoPrime(const std::shared_ptr<ASTNode>& parent);

        void insert(const std::shared_ptr<ASTNode>& parent);

        void resultado(const std::shared_ptr<ASTNode>& parent);

        void listaColumnasPrime(const std::shared_ptr<ASTNode>& parent);

        void listaValores(const std::shared_ptr<ASTNode>& parent);

        void valor(const std::shared_ptr<ASTNode>& parent);

        void createDatabase(const std::shared_ptr<ASTNode>& parent);

        void createTable(const std::shared_ptr<ASTNode>& parent);

        void listaColumnasDef(const std::shared_ptr<ASTNode>& parent);

        void tipoColumna(const std::shared_ptr<ASTNode>& parent);

        void expresionCondicional(const std::shared_ptr<ASTNode>& parent);

        void expresionCondicionalPrime(const std::shared_ptr<ASTNode>& parent);

        void operadorRelacional();

        void termino(const std::shared_ptr<ASTNode>& parent);


        bool match(TokenType type);

        bool check(TokenType type);

        void consume(TokenType type, const std::string& message);

        Token advance();

        Token peek() const;

        Token previous() const;

        bool isAtEnd() const;
};

#endif