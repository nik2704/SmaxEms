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

class ResponseHelper {
public:
    static ResponseHelper& getInstance(
        const std::string& full_path,
        const std::string& json_subfolder,
        std::shared_ptr<std::vector<std::string>> json_action_fields_list,
        std::string attachment_field
    );

    bool dumpJson(const std::string& json_str, const std::string& output_method);
    bool dumpJson(json dblf, const std::string& output_method);

    void printAttachmentsConsole(const std::string& json_str);

    std::shared_ptr<std::vector<Attachment>> getAttachmentInfo(const std::string& jsonString);

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

    void convertFieldsToJson(json& dblf);

    bool validateJson(const json& dblf);

    void printToConsole(const json& dblf);

    bool saveToFile(const json& dblf);

    bool writeToFile(const fs::path& file_path, const json& entity);
};

} // namespace smax_ns