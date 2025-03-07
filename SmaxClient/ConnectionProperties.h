#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

namespace smax_ns {

/**
 * @brief Enumeration representing different actions that can be performed.
 */
enum class Action { GET, CREATE, UPDATE, JSON, GETATTACHMENTS, UNKNOW };

/**
 * @brief Parses a string into an Action enum.
 * @param action_str The string representation of the action.
 * @return The corresponding Action enum value.
 */
inline Action parseAction(const std::string& action_str);

/**
 * @brief Splits a given string by a specified delimiter.
 * @param input The input string to split.
 * @param delimiter The character used to split the string.
 * @return A shared pointer to a vector containing the split parts.
 */
inline std::shared_ptr<std::vector<std::string>> splitString(const std::string& input, char delimiter = ',');

/**
 * @brief Structure representing input values for connection parameters.
 */
struct InputValues {
    std::string protocol;           ///< Protocol (e.g., HTTP or HTTPS)
    std::string host;               ///< SMAX host address
    uint16_t port;                  ///< SMAX port number
    uint16_t secure_port;           ///< Secure SMAX port number
    std::size_t tenant;             ///< Tenant ID
    std::string entity;             ///< Entity Name
    std::string layout;             ///< Layout of fields
    std::string username;           ///< User name for authentication
    std::string password;           ///< Password for authentication
    std::string filter;             ///< Filter for data retrieval
    std::string action;             ///< Action (GET, CREATE, UPDATE, JSON, GETATTACHMENTS)
    std::string csv;                ///< CSV file path for update or create operations
    std::string output_folder;      ///< Output folder for storing results
    bool verbose;                   ///< Verbose mode flag
    std::string json_action_field;  ///< JSON action field name
    std::string json_action_output; ///< Output data for JSON actions
    std::string json_action_output_folder; ///< Folder for JSON action output
    std::string att_action_output;  ///< Output method for attachments (file or console)
    std::string att_action_field;   ///< Attachment action field name
    std::string att_action_output_folder; ///< Folder for storing attachment outputs
};

/**
 * @brief Singleton class to manage connection parameters.
 */
class ConnectionParameters {
public:
    /**
     * @brief Retrieves the singleton instance of ConnectionParameters.
     * @param input_values The input values to initialize the instance.
     * @return Reference to the singleton instance.
     */
    static ConnectionParameters& getInstance(const InputValues& input_values);

    /**
     * @brief Retrieves the existing singleton instance.
     * @return Reference to the singleton instance.
     * @throws std::runtime_error if the instance is not initialized.
     */
    static ConnectionParameters& getInstance();

    /** @brief Retrieves the protocol. */
    const std::string& getProtocol() const;
    /** @brief Retrieves the host. */
    const std::string& getHost() const;
    /** @brief Retrieves the port number. */
    uint16_t getPort() const;
    /** @brief Retrieves the secure port number. */
    uint16_t getSecurePort() const;
    /** @brief Retrieves the tenant ID. */
    std::size_t getTenant() const;
    /** @brief Retrieves the entity name. */
    const std::string& getEntity() const;
    /** @brief Retrieves the layout. */
    const std::string& getLayout() const;
    /** @brief Retrieves the filter string. */
    const std::string& getFilter() const;
    /** @brief Retrieves the username. */
    const std::string& getUserName() const;
    /** @brief Retrieves the password. */
    const std::string& getPassword() const;
    /** @brief Retrieves the action as an enum. */
    const Action& getAction() const;
    /** @brief Retrieves the action as a string. */
    std::string getActionAsString() const;
    /** @brief Retrieves the CSV filename. */
    const std::string& getCSVfilename() const;
    /** @brief Retrieves the output folder path. */
    const std::string& getOutputFolder() const;
    /** @brief Checks if verbose mode is enabled. */
    bool isVerbose() const;
    /** @brief Retrieves the JSON action field. */
    const std::string& getJsonActionField() const;
    /** @brief Retrieves the JSON action output. */
    const std::string& getJsonActionOutput() const;
    /** @brief Retrieves the JSON action output folder. */
    const std::string& getJsonActionOutputFolder() const;
    /** @brief Retrieves the JSON action fields list. */
    const std::shared_ptr<std::vector<std::string>>& getJsonActionFieldsList() const;
    /** @brief Retrieves the attachment action output. */
    const std::string& getAttActionOutput() const;
    /** @brief Retrieves the attachment action field. */
    const std::string& getAttActionField() const;
    /** @brief Retrieves the attachment action output folder. */
    const std::string& getAttActionOutputFolder() const;

    /**
     * @brief Converts an Action enum to its string representation.
     * @param action The Action enum value.
     * @return The string representation of the action.
     */
    static std::string actionToString(Action action);
    
    /**
     * @brief Overloaded stream output operator for ConnectionParameters.
     * @param os Output stream reference.
     * @param params ConnectionParameters instance.
     * @return Reference to the output stream.
     */
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
    /**
     * @brief Constructs the ConnectionParameters instance.
     * @param input_values Input values for initialization.
     */
    explicit ConnectionParameters(const InputValues& input_values);

    ConnectionParameters(const ConnectionParameters&) = delete;
    ConnectionParameters& operator=(const ConnectionParameters&) = delete;

    static std::unique_ptr<ConnectionParameters> instance_; ///< Singleton instance
    static std::once_flag init_flag_; ///< Flag for thread-safe initialization

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
    std::string output_folder_;
    bool verbose_;
    std::string json_action_field_;
    std::shared_ptr<std::vector<std::string>> json_action_fields_list_;
    std::string json_action_output_;
    std::string json_action_output_folder_;
    std::string att_action_output_;
    std::string att_action_field_;
    std::string att_action_output_folder_;
};

} // namespace smax_ns
