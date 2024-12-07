#include "MetadataManager.h"
#include <fstream>
#include <iostream>

MetadataManager::MetadataManager(const std::string& schema)
    : schema_name(schema) {
    std::string folderName = "database";
    std::string folderPathStr = fs::current_path().string(); // Directorio actual
    base_path = folderPathStr + "/" + folderName;
    metadata_file = base_path + "/" + schema_name + "_metadata.meta";
}

bool MetadataManager::writeMetadata(const std::vector<std::string>& fields) {
    std::ofstream meta_file(metadata_file, std::ios::trunc); // Truncar para sobrescribir
    if (!meta_file.is_open()) {
        std::cerr << "Error al abrir el archivo de metadatos para escribir." << std::endl;
        return false;
    }

    for (const auto& field : fields) {
        meta_file << field << std::endl; // Escribir cada campo
    }

    meta_file.close();
    return true;
}

std::vector<std::string> MetadataManager::readMetadata() {
    std::vector<std::string> fields;
    std::ifstream meta_file(metadata_file);
    if (!meta_file.is_open()) {
        std::cerr << "Error al abrir el archivo de metadatos para leer." << std::endl;
        return fields;
    }

    std::string line;
    while (std::getline(meta_file, line)) {
        fields.push_back(line); // Leer cada línea como un campo
    }

    meta_file.close();
    return fields;
}

bool MetadataManager::updateMetadata(const std::string& old_field, const std::string& new_field) {
    std::vector<std::string> fields = readMetadata();
    bool updated = false;

    std::ofstream meta_file(metadata_file, std::ios::trunc);
    if (!meta_file.is_open()) {
        std::cerr << "Error al abrir el archivo de metadatos para actualizar." << std::endl;
        return false;
    }

    for (auto& field : fields) {
        if (field == old_field) {
            field = new_field; // Actualizar el campo
            updated = true;
        }
        meta_file << field << std::endl; // Escribir de nuevo
    }

    meta_file.close();
    return updated;
}

bool MetadataManager::removeField(const std::string& field_to_remove) {
    std::vector<std::string> fields = readMetadata();
    bool removed = false;

    std::ofstream meta_file(metadata_file, std::ios::trunc);
    if (!meta_file.is_open()) {
        std::cerr << "Error al abrir el archivo de metadatos para eliminar un campo." << std::endl;
        return false;
    }

    for (const auto& field : fields) {
        if (field != field_to_remove) {
            meta_file << field << std::endl;
        } else {
            removed = true; // Marcar como eliminado
        }
    }

    meta_file.close();
    return removed;
}

///////////////////////////////////////////////////////////////////////////

#include "MetadataManager.h"
#include <iostream>

int main() {
    MetadataManager mm("schema1");

    // Escribir metadatos
    mm.writeMetadata({"id:int", "name:string", "age:int"});

    // Leer metadatos
    std::vector<std::string> metadata = mm.readMetadata();
    for (const auto& field : metadata) {
        std::cout << "Campo: " << field << std::endl;
    }

    // Actualizar un campo
    mm.updateMetadata("age:int", "age:float");

    // Eliminar un campo
    mm.removeField("name:string");

    return 0;
}

