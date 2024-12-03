#ifndef MetadataManager_h
#define MetadataManager_h

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class MetadataManager {
private:
    std::string base_path;
    
public:
    MetadataManager();
    bool create_schema(std::string schema_name);
};

#endif 
