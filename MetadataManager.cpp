#include "MetadataManager.hpp"

MetadataManager::MetadataManager(){
    std::string folderName = "database";
    std::string folderPathStr = fs::current_path().string(); // Directorio actual
    base_path = folderPathStr + "/" +folderName;  
  
    try {
        fs::path folderPath = base_path;
        
        if (fs::create_directory(folderPath)) {
            std::cout << "Main directory created successfully: " << folderName << std::endl;
        } else {
            std::cout << "Main Directory already exists: " << folderName << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating main directory: " << e.what() << std::endl;
    }
}

bool MetadataManager::create_schema(std::string schema_name){
    fs::path folderPath = base_path + "/"+ schema_name;
    
    try {
        if (fs::create_directory(folderPath)) {
            std::cout << "Schema created successfully: " << schema_name << std::endl;
        } else {
            std::cout << "Schema already exists: " << schema_name << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating Schema directory: " << e.what() << std::endl;
    }
    
    std::ofstream data_file((base_path + "/"+ schema_name + "/" + schema_name + "_metadata.dat"), std::ios::app);  // Abrir en modo de agregar
    if (!data_file.is_open()) {
        std::cerr << "Error al crear _metadata" << std::endl;
        return false;
    }
    data_file.close();
    return true;
}
