#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <mutex>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace smax_ns {

/**
 * @struct Attachment
 * @brief Represents an attachment with metadata.
 */
struct Attachment {
    std::string record_id;
    std::string id;
    std::string file_name;
    std::string file_extension;
    bool is_hidden;
};

/**
 * @class ResponseHelper
 * @brief A singleton class that provides JSON processing and attachment management functionalities.
 */
class ResponseHelper {
public:
    /**
     * @brief Gets the singleton instance of ResponseHelper.
     * @param full_path Base path for file storage.
     * @param json_subfolder Subfolder for JSON storage.
     * @param json_action_fields_list List of fields that need to be converted to JSON.
     * @param attachment_field Name of the attachment field.
     * @return Reference to the singleton instance.
     */
    static ResponseHelper& getInstance(
        const std::string& full_path,
        const std::string& json_subfolder,
        std::shared_ptr<std::vector<std::string>> json_action_fields_list,
        std::string attachment_field
    );

    /**
     * @brief Prints JSON to console of file.
     * @param json_str JSON string.
     * @param output_method output method (console or file).
     * @return true or false.
     */
    bool dumpJson(const std::string& json_str, const std::string& output_method);

    /**
     * @brief Prints JSON to console of file.
     * @param dblf JSON object.
     * @param output_method output method (console or file).
     * @return true or false.
     */
    bool dumpJson(json dblf, const std::string& output_method);

    /**
     * @brief Prints an attchment section (JSON string) to console.
     * @param json_str attachment section.
     */
    void printAttachmentsConsole(const std::string& json_str);

    /**
     * @brief Returns pointer to a vector contains attachment data (Attachment) .
     * @param jsonString attachment section.
     * @return vector of Attachment elements.
     */
    std::shared_ptr<std::vector<Attachment>> getAttachmentInfo(const std::string& jsonString);

    /**
     * @brief Preparation of directory structure.
     * @param subfolder_name subfolder of the folder defined in full_path of the method getInstance.
     * @return path.
     */
    fs::path prepareDirectory(const std::string& subfolder_name);

private:
    fs::path base_path_;
    std::string json_subfolder_;
    std::mutex mutex_;
    std::shared_ptr<std::vector<std::string>> json_action_fields_list_;
    std::string attachment_field_;

    explicit ResponseHelper(
        const std::string& base_path,
        const std::string& json_subfolder,
        std::shared_ptr<std::vector<std::string>> json_action_fields_list,
        std::string attachment_field
    );

    ResponseHelper(const ResponseHelper&) = delete;
    ResponseHelper& operator=(const ResponseHelper&) = delete;

    /**
     * @brief Converts field (json is string format, defined in json_action_fields_list) into a json object.
     * @param dblf main json.
     * @return true or false.
     */
    void convertFieldsToJson(json& dblf);

    /**
     * @brief Checks that json contains the element entities and entities is an array.
     * @param dblf json to check.
     * @return true or false.
     */
    bool validateJson(const json& dblf);

    /**
     * @brief Prints JSON to console.
     * @param dblf json to check.
     */
    void printToConsole(const json& dblf);

    /**
     * @brief Saves JSON field in a text file.
     * @param dblf json to save.
     * @return true or false.
     */
    bool saveToFile(const json& dblf);

    /**
     * @brief Writes JSON element to a file.
     * @param file_path full file name.
     * @param entity json element.
     * @return true or false.
     */
    bool writeToFile(const fs::path& file_path, const json& entity);
};

} // namespace smax_ns