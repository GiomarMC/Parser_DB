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
    std::string base_path;
    std::string current_schema;

    std::map<std::streampos, size_t> free_space;

public:
    FileManager();
    bool create_schema(std::string schema_name);
    bool create_file(const std::string& filename);
    bool delete_file(const std::string& filename);
    void list_files();
    bool write_record(const std::string& filename, const std::string& record);
    std::string read_record(const std::string& filename, size_t index);
    void manage_free_space(const std::string& filename);

};

#endif