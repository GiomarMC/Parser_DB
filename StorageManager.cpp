#include "StorageManager.h"
#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

StorageManager::StorageManager(const std::string& schema, const std::string& meta, const std::string& index)
    : schema_name(schema), metadata(meta), index_file(index) {
    // Ruta completa al directorio de la base de datos
    base_path = fs::current_path().string() + "/database/" + schema_name;
}

bool StorageManager::write(const std::string& table, const std::string& data) {
    std::string dataFilePath = base_path + "/" + table + ".dat";
    std::string indexFilePath = base_path + "/" + table + ".idx";

    std::ofstream data_file(dataFilePath, std::ios::app);
    std::ofstream idx_file(indexFilePath, std::ios::app);

    if (!data_file.is_open() || !idx_file.is_open()) {
        std::cerr << "Error al abrir archivo de datos o índice para escribir." << std::endl;
        return false;
    }

    std::streampos pos = data_file.tellp();
    data_file << data << std::endl;
    idx_file << pos << std::endl;

    data_file.close();
    idx_file.close();
    return true;
}

std::vector<std::string> StorageManager::read() {
    std::vector<std::string> records;
    std::ifstream data_file(base_path + "/" + schema_name + ".dat");

    if (!data_file.is_open()) {
        std::cerr << "Error al abrir el archivo para leer." << std::endl;
        return records;
    }

    std::string line;
    while (std::getline(data_file, line)) {
        records.push_back(line);
    }
    data_file.close();
    return records;
}

bool StorageManager::update(const std::string& old_data, const std::string& new_data) {
    try {
        fs::path dataFilePath = fs::path(base_path) / (schema_name + "_data.dat");
        std::vector<std::string> records = read();

        bool updated = false;
        std::ofstream dataFile(dataFilePath, std::ios::trunc);

        if (!dataFile.is_open()) {
            throw std::ios::failure("Error abriendo archivo para actualizar.");
        }

        for (auto& record : records) {
            if (record == old_data) {
                record = new_data;
                updated = true;
            }
            dataFile << record << "\n";
        }
        return updated;

    } catch (const std::exception& e) {
        std::cerr << "Error actualizando registro: " << e.what() << std::endl;
        return false;
    }
}

bool StorageManager::remove(const std::string& data) {
    try {
        fs::path dataFilePath = fs::path(base_path) / (schema_name + "_data.dat");
        fs::path idxFilePath = fs::path(base_path) / (schema_name + "_data.idx");

        std::vector<std::string> records = read();
        std::ofstream dataFile(dataFilePath, std::ios::trunc);
        std::ofstream idxFile(idxFilePath, std::ios::trunc);

        if (!dataFile.is_open() || !idxFile.is_open()) {
            throw std::ios::failure("Error al abrir archivos para eliminar.");
        }

        for (size_t i = 0; i < records.size(); ++i) {
            if (records[i] != data) {
                dataFile << records[i] << "\n";
                idxFile << i << "\n";
            }
        }
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error eliminando registro: " << e.what() << std::endl;
        return false;
    }
}

bool StorageManager::manageIndex(const std::string& index_data) {
    std::ofstream index_stream(base_path +"/"+ index_file, std::ios::app);
    if (!index_stream.is_open()) {
        std::cerr << "Error al abrir el archivo de índice." << std::endl;
        return false;
    }
    index_stream << index_data << std::endl;  // Añadir índice
    index_stream.close();
    return true;
}

std::vector<std::string> StorageManager::readIndex() {
    std::vector<std::string> indices;
    std::ifstream index_stream(base_path +"/"+ index_file);
    if (!index_stream.is_open()) {
        std::cerr << "Error al abrir el archivo de índices." << std::endl;
        return indices;
    }

    std::string line;
    while (std::getline(index_stream, line)) {
        indices.push_back(line);  // Almacenar cada índice
    }
    index_stream.close();
    return indices;
}

// Compare this snippet from MetadataManager.h:
// #ifndef METADATA_MANAGER_H
// #define METADATA_MANAGER_H
//
/////////////////////////////////////////////////////////////////////////

#include "StorageManager.h"
/*
int main() {
    StorageManager sm("schema1", "metadata", "index.idx");

    // Escribir datos
    sm.write("record1");
    sm.write("record2");

    // Leer datos
    std::vector<std::string> data = sm.read();
    for (const auto& record : data) {
        std::cout << "Registro: " << record << std::endl;
    }

    // Actualizar datos
    sm.update("record1", "updated_record1");

    // Eliminar datos
    sm.remove("record2");

    // Gestionar índices
    sm.manageIndex("index1");

    return 0;
}
*/