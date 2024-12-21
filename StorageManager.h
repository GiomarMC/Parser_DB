#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <string>
#include <vector>

class StorageManager {
private:
    std::string base_path;
    std::string schema_name;   // Esquema activo
    std::string metadata;      // Información asociada al esquema
    std::string index_file;    // Ruta al archivo de índices

public:
    // Constructor
    StorageManager(const std::string& schema, const std::string& meta, const std::string& index);

    // Métodos para manejar registros
    bool write(const std::string& table, const std::string& data);
    std::vector<std::string> read();
    bool update(const std::string& old_data, const std::string& new_data);
    bool remove(const std::string& data);

    // Métodos para manejar índices
    bool manageIndex(const std::string& index_data);
    std::vector<std::string> readIndex();
};

#endif // STORAGE_MANAGER_H
