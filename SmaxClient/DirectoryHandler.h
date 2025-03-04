#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace smax_ns {

class DirectoryHandler {
public:
    static DirectoryHandler& getInstance(const std::string& full_path) {
        static DirectoryHandler instance(full_path);
        return instance;
    }

    bool isFolderAvailable(const std::string& subfolder_name) {
        std::lock_guard<std::mutex> lock(mutex_);
        fs::path subfolder_path = base_path / subfolder_name;
        if (!fs::exists(subfolder_path)) {
            fs::create_directories(subfolder_path);
            return false;
        }
        return fs::is_empty(subfolder_path);
    }

    bool dumpJson(const json& dblf, const std::string& subfolder_name, const std::string& output_method) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            if (!dblf.contains("entities") || !dblf["entities"].is_array()) {
                std::cerr << "Error: JSON does not contain 'entities' array." << std::endl;
                return false;
            }
            
            fs::path subfolder_path = base_path / subfolder_name;
            if (!fs::exists(subfolder_path)) {
                fs::create_directories(subfolder_path);
            }
            
            if (output_method == "console") {
                std::cout << dblf.dump(4) << std::endl;
            } else if (output_method == "file") {
                for (const auto& entity : dblf["entities"]) {
                    if (!entity["properties"].contains("Id")) {
                        std::cerr << "Error: Entity does not contain 'Id' property." << std::endl;
                        continue;
                    }
                    
                    std::string id = entity["properties"]["Id"].get<std::string>();
                    fs::path file_path = subfolder_path / (id + ".json");
                    
                    std::ofstream out_file(file_path);
                    if (!out_file) {
                        std::cerr << "Error: Could not create file " << file_path << std::endl;
                        return false;
                    }
                    out_file << entity.dump(4);
                    out_file.close();
                }
            } else {
                std::cerr << "Error: Invalid output method." << std::endl;
                return false;
            }
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            return false;
        }
    }

private:
    fs::path base_path;
    std::mutex mutex_;

    explicit DirectoryHandler(const std::string& full_path) : base_path(full_path) {
        if (!fs::exists(base_path)) {
            fs::create_directories(base_path);
        }
    }
    
    DirectoryHandler(const DirectoryHandler&) = delete;
    DirectoryHandler& operator=(const DirectoryHandler&) = delete;
};

} // namespase smax_ns