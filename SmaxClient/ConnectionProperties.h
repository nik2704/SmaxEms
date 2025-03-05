#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

namespace smax_ns {

enum class Action { GET, CREATE, UPDATE, JSON, GETATTACHMENTS, UNKNOW };

inline Action parseAction(const std::string& action_str);
inline std::shared_ptr<std::vector<std::string>> splitString(const std::string& input, char delimiter  = ',');

struct InputValues {
    std::string protocol;           ///< Protocol
    std::string host;               ///< SMAX host
    uint16_t port;                  ///< SMAX port
    uint16_t secure_port;           ///< SMAX secure port
    std::size_t tenant;             ///< Tenant ID
    std::string entity;             ///< Entity Name
    std::string layout;             ///< Layout (fields)
    std::string username;           ///< User name
    std::string password;           ///< Password
    std::string filter;             ///< Filter
    std::string action;             ///< Action (GET | CREATE | UPDATE | JSON | GETATTACHMENTS)
    std::string csv;                ///< CSV for update or create records
    bool verbose;                   ///< Just information
    std::string json_action_field;          ///< RequestAttachments, TaskPlanForApprove,TaskPlanForClassify,TaskPlanForEscalate,TaskPlanForFirstLineSupport,TaskPlanForFulfill,TaskPlanForReview
    std::string json_action_output;         ///< console or file
    std::string json_action_output_folder;  ///< output folder
    std::string att_action_output;          ///< file or console
    std::string att_action_output_mode;     ///< list or content
    std::string att_action_field;           ///< like RequestAttachments
    std::string att_action_output_folder;   ///< output folder
};

class ConnectionParameters {
public:
    static ConnectionParameters& getInstance(const InputValues& input_values);
    static ConnectionParameters& getInstance();

    const std::string& getProtocol() const;
    const std::string& getHost() const;
    uint16_t getPort() const;
    uint16_t getSecurePort() const;
    std::size_t getTenant() const;
    const std::string& getEntity() const;
    const std::string& getLayout() const;
    const std::string& getFilter() const;
    const std::string& getUserName() const;
    const std::string& getPassword() const;
    const Action& getAction() const;
    std::string getActionAsString() const;
    const std::string& getCSVfilename() const;
    bool isVerbose() const;
    const std::string& getJsonActionField() const;
    const std::string& getJsonActionOutput() const;
    const std::string& getJsonActionOutputFolder() const;
    const std::shared_ptr<std::vector<std::string>>& getJsonActionFieldsList() const;
    const std::string& getAttActionOutput() const;
    const std::string& getAttActionOutputMode() const;
    const std::string& getAttActionField() const;
    const std::string& getAttActionOutputFolder() const;

    friend std::ostream& operator<<(std::ostream& os, const ConnectionParameters& params) {
        os << "Protocol: " << params.protocol_ << "\n"
           << "Host: " << params.host_ << "\n"
           << "Secure Port: " << params.secure_port_ << "\n"
           << "Port: " << params.port_ << "\n"
           << "Tenant: " << params.tenant_ << "\n"
           << "Entity: " << params.entity_ << "\n"
           << "Layout: " << params.layout_ << "\n";
        return os;
    }

private:
    explicit ConnectionParameters(const InputValues& input_values);
    ConnectionParameters(const ConnectionParameters&) = delete;
    ConnectionParameters& operator=(const ConnectionParameters&) = delete;

    static std::unique_ptr<ConnectionParameters> instance_;
    static std::once_flag init_flag_;

    std::string protocol_;
    std::string host_;
    uint16_t port_;
    uint16_t secure_port_;
    std::size_t tenant_;
    std::string entity_;
    std::string layout_;
    std::string username_;
    std::string password_;
    std::string filter_;
    Action action_;
    std::string csv_;
    bool verbose_;
    std::string json_action_field_;
    std::shared_ptr<std::vector<std::string>> json_action_fields_list_;
    std::string json_action_output_;
    std::string json_action_output_folder_;
    std::string att_action_output_;
    std::string att_action_output_mode_;
    std::string att_action_field_;
    std::string att_action_output_folder_;


    static std::string actionToString(Action action);
};

} // namespace smax_ns