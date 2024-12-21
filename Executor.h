#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "Parser.h"
#include "FileManager.h"
#include "StorageManager.h"
#include "MetadataManager.h"
#include <iostream>
#include <vector>
#include <stdexcept>

class Executor
{
    private:
        FileManager fileManager;
        StorageManager storageManager;
        MetadataManager metadataManager;
        std::string currentDatabase;

    public:
        Executor(const std::string& schema);

        void execute(const std::shared_ptr<ASTNode>& root);

    private:
        void handleCreateDatabase(const std::shared_ptr<ASTNode>& root);
        void handleCreateTable(const std::shared_ptr<ASTNode>& root);
        void handleInsert(const std::shared_ptr<ASTNode>& root);
        void handleSelect(const std::shared_ptr<ASTNode>& root);
        bool evaluateCondition(const std::string& record, const std::string& left, 
                                    const std::string& operador, const std::string& right);

        std::string serializeRecord(const std::vector<std::string>& columns, const std::vector<std::string>& values);
        void printRecords(const std::vector<std::string>& records, const std::vector<std::string>& columns);
        void printAST(const std::shared_ptr<ASTNode>& node, int depth);
};

#endif