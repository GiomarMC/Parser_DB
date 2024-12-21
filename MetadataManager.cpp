#include "MetadataManager.h"
#include <fstream>
#include <iostream>

MetadataManager::MetadataManager(const std::string& schema)
    : schema_name(schema) {
    base_path = fs::current_path().string() + "/database/" + schema_name;
    metadata_file = base_path + "/metadata.meta"; // Cambiar a un formato único por base de datos
}

void MetadataManager::setMetadataFile(const std::string& schema, const std::string& tableName) {
    schema_name = schema;
    metadata_file = "database/" + schema_name + "/" + tableName + "_metadata.meta";
}

bool MetadataManager::writeMetadata(const std::vector<std::string>& fields) {
    std::ofstream meta_file(metadata_file, std::ios::trunc);
    if (!meta_file.is_open()) {
        std::cerr << "Error al abrir el archivo de metadatos para escribir." << std::endl;
        return false;
    }

    for (const auto& field : fields) {
        meta_file << field << std::endl;
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
        fields.push_back(line);
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