#ifndef METADATA_MANAGER_H
#define METADATA_MANAGER_H

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class MetadataManager {
private:
    std::string schema_name;     // Nombre del esquema
    std::string base_path;       // Ruta base para almacenar archivos
    std::string metadata_file;   // Nombre del archivo de metadatos

public:
    MetadataManager(const std::string& schema);

    // Escribe los campos de metadatos en el archivo
    bool writeMetadata(const std::vector<std::string>& fields);

    // Establece el archivo de metadatos
    void setMetadataFile(const std::string& schema, const std::string& tableName);

    // Lee los campos de metadatos desde el archivo
    std::vector<std::string> readMetadata();

    // Actualiza un campo de metadatos
    bool updateMetadata(const std::string& old_field, const std::string& new_field);

    // Elimina un campo de metadatos
    bool removeField(const std::string& field_to_remove);
};

#endif
