#include "Executor.h"

Executor::Executor(const std::string& schema)
    : currentDatabase(schema), 
      fileManager(),
      storageManager(schema, "metadata", "index.idx"),
      metadataManager(schema)
{}

void Executor::execute(const std::shared_ptr<ASTNode>& root)
{
    try {
        if (root->name == "Programa" || root->name == "ListaSentencias") {
            for (const auto& child : root->children)
                execute(child);
        } else if (root->name == "Sentencia") {
            auto commandNode = root->children[0];
            std::cout << "Comando detectado: " << commandNode->name << std::endl;

            if (commandNode->name == "CREATE") {
                if (commandNode->children[0]->name == "DATABASE")
                    handleCreateDatabase(commandNode->children[0]);
                else if (commandNode->children[0]->name == "TABLE")
                    handleCreateTable(commandNode->children[0]);
                else
                    throw std::runtime_error("Error: Comando CREATE no soportado.");
            }
            else if (commandNode->name == "INSERT")
                handleInsert(commandNode);
            else if (commandNode->name == "SELECT")
                handleSelect(commandNode);
            else
                throw std::runtime_error("Error: Comando no soportado.");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en ejecución: " << e.what() << std::endl;
    }
}

void Executor::handleCreateDatabase(const std::shared_ptr<ASTNode>& root)
{
    currentDatabase = root->children[0]->name;
    if (fileManager.create_schema(currentDatabase))
    {
        std::cout << "Base de datos '" << currentDatabase << "' creada con éxito.\n";
    }
    else
    {
        throw std::runtime_error("Error: No se pudo crear la base de datos '" + currentDatabase + "'.");
    }
}

void Executor::handleCreateTable(const std::shared_ptr<ASTNode>& root)
{
    std::string tableName = root->children[0]->name;
    std::vector<std::string> columns;

    auto columnsNode = root->children[1];
    for (const auto& columnNode : columnsNode->children)
    {
        std::string columnName = columnNode->children[0]->name;
        std::string columnType = columnNode->children[1]->name;
        columns.push_back(columnName + ":" + columnType);
    }

    if (!fileManager.create_file(tableName))
        throw std::runtime_error("Error: No se pudo crear los archivos para la tabla '" + tableName + "'.");

    metadataManager.setMetadataFile(currentDatabase, tableName);
    if (metadataManager.writeMetadata(columns))
        std::cout << "Tabla '" << tableName << "' creada con éxito.\n";
    else
        throw std::runtime_error("Error: No se pudo crear la tabla '" + tableName + "'.");
}

void Executor::handleInsert(const std::shared_ptr<ASTNode>& root)
{
    std::string tableName = root->children[0]->name;

    std::vector<std::string> columns, values;

    auto columnsNode = root->children[1];

    for (const auto& colNode : columnsNode->children)
    {
        if (!colNode->children.empty()) {
            std::string columnName = colNode->children[0]->name;
            columns.push_back(columnName);
        }
    }

    auto valuesNode = root->children[2];

    for (const auto& valNode : valuesNode->children)
    {
        if (!valNode->children.empty()) {
            std::string value = valNode->children[0]->name;
            values.push_back(value);
        }
    }

    metadataManager.setMetadataFile(currentDatabase, tableName);
    auto schema = metadataManager.readMetadata();
    for (const auto& field : schema)
        std::cout << " - " << field << std::endl;

    if (values.size() != schema.size())
    {
        std::cerr << "Error: Tamaño del esquema (" << schema.size() 
                  << ") no coincide con columnas especificadas (" << columns.size() << ")." << std::endl;
        throw std::runtime_error("Error: El esquema no coincide con las columnas especificadas.");
    }

    storageManager = StorageManager(currentDatabase, "metadata", "index.idx");

    std::string record = serializeRecord(columns, values);
    if (storageManager.write(tableName,record))
        std::cout << "Registro insertado correctamente: " << record << std::endl;
    else
        throw std::runtime_error("Error: No se pudo insertar el registro.");

    std::cout << "--- FIN DE handleInsert ---\n" << std::endl;
}

void Executor::handleSelect(const std::shared_ptr<ASTNode>& root)
{
    std::string tableName = root->children[1]->name;
    std::string metadataPath = "database/" + currentDatabase + "/" + tableName + "_metadata.meta";

    metadataManager = MetadataManager(metadataPath);
    auto columns = metadataManager.readMetadata();
    auto records = storageManager.read();

    auto condicionResultadoNode = root->children[2];
    if (condicionResultadoNode->name == "Condicion")
    {
        auto expresionCondicionalNode = condicionResultadoNode->children[0];
        std::string left = expresionCondicionalNode->children[0]->name;
        std::string operador = expresionCondicionalNode->children[1]->name;
        std::string right = expresionCondicionalNode->children[2]->name;

        std::vector<std::string> filteredRecords;
        for (const auto& record : records)
        {
            if (evaluateCondition(record, left, operador, right))
                filteredRecords.push_back(record);
        }
        records = filteredRecords;
    }

    printRecords(records, columns);
}

std::string Executor::serializeRecord(const std::vector<std::string>& columns, const std::vector<std::string>& values)
{
    std::string record;
    for (size_t i = 0; i < columns.size(); ++i)
    {
        record += columns[i] + "=" + values[i] + ";";
    }
    return record;
}

void Executor::printRecords(const std::vector<std::string>& records, const std::vector<std::string>& columns)
{
    for (const auto& col : columns)
        std::cout << col << "\t";
    std::cout << "\n----------------------------------\n";

    for (const auto& record : records)
    {
        std::cout << record << "\n";
    }
}

bool Executor::evaluateCondition(const std::string& record, const std::string& left, 
                                 const std::string& operador, const std::string& right)
{
    std::map<std::string, std::string> recordMap;
    std::istringstream recordStream(record);
    std::string pair;
    while (std::getline(recordStream, pair, ';'))
    {
        auto delimiterPos = pair.find('=');
        if (delimiterPos != std::string::npos)
        {
            std::string key = pair.substr(0, delimiterPos);
            std::string value = pair.substr(delimiterPos + 1);
            recordMap[key] = value;
        }
    }

    if (recordMap.find(left) == recordMap.end())
        return false;

    std::string recordValue = recordMap[left];
    return recordValue == right;
}