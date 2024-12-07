#include <iostream>
#include "FileManager.h"
#include "StorageManager.h"
#include "MetadataManager.h"

namespace fs = std::filesystem;

void testFileManager() {
    FileManager filemanager;
    filemanager.create_schema("test_schema");
    filemanager.write_record("test_file", "Registro 1");
    filemanager.write_record("test_file", "Registro 2");
    std::cout << "Leyendo registros:\n";
    std::cout << filemanager.read_record("test_file", 0) << std::endl;
    std::cout << filemanager.read_record("test_file", 1) << std::endl;
    
//    std::cout << "list_files" << std::endl;
//    filemanager.list_files();
}

void testStorageManager(){
    std::cout << "STORAGE" << std::endl;
    // STORAGEM
    StorageManager sm("schema1", "metadata", "index.idx");
    // Escribir datos
    sm.write("record1");
    sm.write("record2");
    // Leer datos
    std::cout << "Init data" << std::endl;
    std::vector<std::string> data = sm.read();
    for (const auto& record : data) {
        std::cout << "Registro: " << record << std::endl;
    }
    // Actualizar datos
    sm.update("record1", "updated_record1");
    // Leer datos
    std::cout << "Updated file" << std::endl;
    data = sm.read();
    for (const auto& record : data) {
        std::cout << "Registro: " << record << std::endl;
    }
    
    // Eliminar datos
    sm.remove("record2");
    std::cout << "Deleted 2" << std::endl;
    data = sm.read();
    for (const auto& record : data) {
        std::cout << "Registro: " << record << std::endl;
    }
    
    // Gestionar índices
    sm.manageIndex("index1");
    
    // Leer datos
    std::cout << "Final Data: " << std::endl;
    data = sm.read();
    for (const auto& record : data) {
        std::cout << "Registro: " << record << std::endl;
    }
}
/*
void testMetadataManager(){
    MetadataManager mm;
    mm.create_schema("schema1");
}
*/
int main(int argc, const char * argv[]) {
    
    testFileManager();
    testStorageManager();
    //testMetadataManager();
    
    return 0;
}
