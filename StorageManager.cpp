#include "StorageManager.h"
#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

StorageManager::StorageManager(const std::string& schema, const std::string& meta, const std::string& index)
    : schema_name(schema), metadata(meta), index_file(index) {
        std::string folderName = "database";
        std::string folderPathStr = fs::current_path().string(); // Directorio actual
        base_path = folderPathStr + "/" + folderName;
    }

bool StorageManager::write(const std::string& data) {
    std::ofstream data_file(base_path +"/"+ schema_name + "_data.dat", std::ios::app);  // Abrir en modo de agregar
    if (!data_file.is_open()) {
        std::cerr << "Error al abrir el archivo para escribir." << std::endl;
        return false;
    }

    data_file << data << std::endl;  // Escribir el registro
    data_file.close();
    return true;
}

std::vector<std::string> StorageManager::read() {
    std::vector<std::string> records;
    std::ifstream data_file(base_path +"/"+ schema_name + "_data.dat");
    if (!data_file.is_open()) {
        std::cerr << "Error al abrir el archivo para leer." << std::endl;
        return records;
    }

    std::string line;
    while (std::getline(data_file, line)) {
        records.push_back(line);  // Almacenar cada registro
    }
    data_file.close();
    return records;
}

bool StorageManager::update(const std::string& old_data, const std::string& new_data) {
    std::vector<std::string> records = read();  // Leer los registros actuales
    bool updated = false;

    std::ofstream data_file(base_path +"/"+ schema_name + "_data.dat", std::ios::trunc);  // Abrir en modo truncar para reemplazar contenido
    if (!data_file.is_open()) {
        std::cerr << "Error al abrir el archivo para actualizar." << std::endl;
        return false;
    }

    for (auto& record : records) {
        if (record == old_data) {
            record = new_data;  // Actualizar el registro
            updated = true;
        }
        data_file << record << std::endl;  // Escribir el registro de vuelta al archivo
    }

    data_file.close();
    return updated;
}

bool StorageManager::remove(const std::string& data) {
    std::vector<std::string> records = read();  // Leer los registros actuales
    bool deleted = false;

    std::ofstream data_file(base_path +"/"+ schema_name + "_data.dat", std::ios::trunc);  // Abrir en modo truncar para reemplazar contenido
    if (!data_file.is_open()) {
        std::cerr << "Error al abrir el archivo para eliminar." << std::endl;
        return false;
    }

    for (auto& record : records) {
        if (record != data) {
            data_file << record << std::endl;  // Escribir el registro que no se elimina
        } else {
            deleted = true;  // Marcar que se eliminó el registro
        }
    }

    data_file.close();
    return deleted;
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