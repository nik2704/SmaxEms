#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace smax_ns {

struct Attachment {
    std::string record_id;
    std::string id;
    std::string file_name;
    std::string file_extension;
    bool is_hidden;
};

class DirectoryHandler {
public:
    static DirectoryHandler& getInstance(
        const std::string& full_path,
        const std::string& json_subfolder,
        std::shared_ptr<std::vector<std::string>> json_action_fields_list,
        std::string attachment_field
    ) {
        static DirectoryHandler instance(full_path, json_subfolder, json_action_fields_list, attachment_field);
        return instance;
    }

    bool dumpJson(const std::string& json_str, const std::string& output_method) {
        try {
            json parsed_json = json::parse(json_str);

            return dumpJson(parsed_json, output_method);
        } catch (const json::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            return false;
        }
    }

    bool dumpJson(json dblf, const std::string& output_method) {
        std::lock_guard<std::mutex> lock(mutex_);

        convertFieldsToJson(dblf);

        if (!validateJson(dblf)) return false;

        if (output_method == "console") {
            printToConsole(dblf);
        } else if (output_method == "file") {
            return saveToFile(dblf);
        } else {
            std::cerr << "Error: Invalid output method." << std::endl;
            return false;
        }

        return true;
    }

    void printAttachmentsConsole(const std::string& json_str) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto attachmentInfo = getAttachmentInfo(json_str);

        for (const auto& att : *attachmentInfo) {
            std::cout << "Record ID:" << att.record_id << ", File ID:" << att.id
                    << ", File Name:" << att.file_name << ", Extension:" << att.file_extension
                    << ", Is Hidden:" << (att.is_hidden ? "true" : "false") << std::endl;
        }
    }

    std::shared_ptr<std::vector<Attachment>> getAttachmentInfo(const std::string& jsonString) {
        auto attachments = std::make_shared<std::vector<Attachment>>();
        try {
            nlohmann::json jsonData = nlohmann::json::parse(jsonString);
            
            for (const auto& entity : jsonData["entities"]) {
                std::string record_id = entity["properties"]["Id"].get<std::string>();
                
                if (entity["properties"].contains(attachment_field_)) {
                    auto attachmentsJson = nlohmann::json::parse(entity["properties"][attachment_field_].get<std::string>());
                    
                    for (const auto& item : attachmentsJson["complexTypeProperties"]) {
                        Attachment att;
                        att.record_id = record_id;
                        att.id = item["properties"]["id"].get<std::string>();
                        att.file_name = item["properties"].value("file_name", "");
                        att.file_extension = item["properties"].value("file_extension", "");
                        att.is_hidden = item["properties"]["IsHidden"].get<bool>();
                        
                        attachments->push_back(att);
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
        
        return attachments;
    }

    fs::path prepareDirectory(const std::string& subfolder_name) {
        if (!fs::exists(base_path_)) {
            fs::create_directories(base_path_);
        }

        fs::path subfolder_path = base_path_ / subfolder_name;

        if (!fs::exists(subfolder_path)) {
            fs::create_directories(subfolder_path);
        }

        return subfolder_path;
    }    

private:
    fs::path base_path_;
    std::string json_subfolder_;
    std::mutex mutex_;
    std::shared_ptr<std::vector<std::string>> json_action_fields_list_;
    std::string attachment_field_;

    explicit DirectoryHandler(
        const std::string& base_path,
        const std::string& json_subfolder,
        std::shared_ptr<std::vector<std::string>> json_action_fields_list,
        std::string attachment_field
    )
        : base_path_(fs::absolute(base_path)), json_subfolder_(json_subfolder), json_action_fields_list_(json_action_fields_list), attachment_field_(attachment_field) { }

    DirectoryHandler(const DirectoryHandler&) = delete;
    DirectoryHandler& operator=(const DirectoryHandler&) = delete;

    void convertFieldsToJson(json& dblf) {
        if (!dblf.contains("entities") || !dblf["entities"].is_array()) return;

        for (auto& entity : dblf["entities"]) {
            if (!entity.contains("properties") || !entity["properties"].is_object()) continue;

            for (const auto& field : *json_action_fields_list_) {
                if (entity["properties"].contains(field) && entity["properties"][field].is_string()) {
                    std::string field_value = entity["properties"][field].get<std::string>();

                    if (!field_value.empty() && field_value.front() == '{' && field_value.back() == '}') {
                        try {
                            json parsed_json = json::parse(field_value);
                            entity["properties"][field] = parsed_json;
                        } catch (const json::exception&) {
                            std::cerr << "Warning: Could not parse field '" << field << "' in entity." << std::endl;
                        }
                    }
                }
            }
        }
    }

    bool validateJson(const json& dblf) {
        if (!dblf.contains("entities") || !dblf["entities"].is_array()) {
            std::cerr << "Error: JSON does not contain 'entities' array." << std::endl;
            return false;
        }
        return true;
    }

    void printToConsole(const json& dblf) {
        std::cout << dblf.dump(4) << std::endl;
    }

    bool saveToFile(const json& dblf) {
        auto subfolder_path = prepareDirectory(json_subfolder_);
        // std::cout << "1" << std::endl;
        // std::cout << dblf.dump(4) << std::endl;

        for (const auto& entity : dblf["entities"]) {
            if (!entity["properties"].contains("Id")) {
                std::cerr << "Error: Entity does not contain 'Id' property." << std::endl;
                continue;
            }

            std::string id = entity["properties"]["Id"].get<std::string>();
            fs::path file_path = subfolder_path / (id + ".json");
            if (!writeToFile(file_path, entity)) return false;
        }
        return true;
    }

    bool writeToFile(const fs::path& file_path, const json& entity) {
        std::ofstream out_file(file_path);
        if (!out_file) {
            std::cerr << "Error: Could not create file " << file_path << std::endl;
            return false;
        }
        out_file << entity.dump(4);
        return true;
    }

};

} // namespace smax_ns