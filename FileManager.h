#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <vector>

namespace fs = std::filesystem;

class FileManager {
private:
    std::string base_path;          //directorio base
    std::string current_schema;     // Esquema actual

// Estructura para espacio libre: mapa de posición inicial y tamaño
    std::map<std::streampos, size_t> free_space;

public:
    FileManager();
    bool create_schema(std::string schema_name);
    bool create_file(const std::string& filename);  // Crea un archivo
    bool delete_file(const std::string& filename);  // Elimina un archivo
    void list_files();                        // Lista los archivos en el esquema actual
    bool write_record(const std::string& filename, const std::string& record); // Escribe datos
    std::string read_record(const std::string& filename, size_t index);        // Lee un registro
    void manage_free_space(const std::string& filename); // Gestiona espacio libre

};

#endif /* FileManager_h */
