
#include "FileManager.h"

FileManager::FileManager() {
    try {
        std::string folderName = "database";
        std::string folderPathStr = fs::current_path().string();
        
        fs::path folderPath = folderPathStr + "/" + folderName;
        base_path = folderPathStr + "/" + folderName;
        
        if (fs::create_directory(folderPath)) {
            std::cout << "Main directory created successfully: " << folderName << std::endl;
        } else {
            std::cout << "Main directory already exists: " << folderName << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating main directory: " << e.what() << std::endl;
    }
}


bool FileManager::create_schema(std::string schema_name){
    
    try {
        fs::path folderPath = base_path + "/"+ schema_name;
        
        if (fs::create_directory(folderPath)) {
            std::cout << "Schema created successfully: " << schema_name << std::endl;
        } else {
            std::cout << "Schema already exists: " << schema_name << std::endl;
        }
        current_schema = schema_name;
        
    } catch (const std::exception& e) {
        std::cerr << "Error creating Schema: " << e.what() << std::endl;
        return false;
    }
    
    return true;
}

bool FileManager::create_file(const std::string& filename) {
    try {
        fs::path filePath = base_path + "/" + current_schema + "/" + filename;

        std::ofstream dataFile(filePath.string() + ".dat");
        std::ofstream metaFile(filePath.string() + "_metadata.meta");
        std::ofstream idxFile(filePath.string() + ".idx");

        if (dataFile && metaFile && idxFile) {
            std::cout << "Archivos creados exitosamente para la tabla: " << filename << std::endl;
            dataFile.close();
            metaFile.close();
            idxFile.close();
            return true;
        } else {
            std::cerr << "Error al crear archivos para la tabla: " << filename << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creando archivos: " << e.what() << std::endl;
        return false;
    }
}

bool FileManager::write_record(const std::string& filename, const std::string& record) {
    try {
        fs::path dataFilePath = fs::path(base_path) / current_schema / (filename + ".dat");
        fs::path idxFilePath = fs::path(base_path) / current_schema / (filename + ".idx");

        std::ofstream dataFile(dataFilePath, std::ios::app);
        std::ofstream idxFile(idxFilePath, std::ios::app);

        if (!dataFile || !idxFile) {
            throw std::ios::failure("No se pudo abrir el archivo para escritura.");
        }

        std::streampos pos = dataFile.tellp();
        dataFile << record << "\n";
        idxFile << pos << "\n";

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error escribiendo registro: " << e.what() << std::endl;
        return false;
    }
}

std::string FileManager::read_record(const std::string& schema_name, size_t record_id) {
    try {
        fs::path idxFilePath = base_path + "/"  + current_schema + "/" + schema_name + ".idx";
        fs::path dataFilePath = base_path + "/"  + current_schema + "/" + schema_name + ".dat";
        
        if (!fs::exists(idxFilePath) || !fs::exists(dataFilePath)) {
            throw std::runtime_error("Index or data file does not exist for schema: " + schema_name);
        }

        std::ifstream idxFile(idxFilePath, std::ios::binary);
        if (!idxFile.is_open()) {
            throw std::runtime_error("Failed to open index file: " + idxFilePath.string());
        }

        std::vector<std::streamoff> positions;
        std::streamoff pos;
        int value;
        while (idxFile >> value){
            pos = static_cast<std::streamoff>(value);
            positions.push_back(pos);
        }

        if (record_id >= positions.size()) {
            throw std::out_of_range("Record ID is out of range.");
        }

        std::streamoff record_position = positions[record_id];

        std::ifstream dataFile(dataFilePath, std::ios::binary);
        if (!dataFile.is_open()) {
            throw std::runtime_error("Failed to open data file: " + dataFilePath.string());
        }

        dataFile.seekg(record_position);
        std::string record;
        std::getline(dataFile, record);

        return record;
    } catch (const std::exception& e) {
        std::cerr << "Error reading record: " << e.what() << std::endl;
        return "";
    }
}

void FileManager::manage_free_space(const std::string& filename) {
    // Implementación básica para rastrear fragmentación
    // Se puede almacenar en un archivo separado si es necesario
    std::cout << "Gestión de espacio libre no implementada aún." << std::endl;
}

void FileManager::list_files(){
    for (const auto& entry : fs::directory_iterator(base_path)) {
        std::cout << entry.path() << std::endl;
    }
}

namespace fs = std::filesystem;