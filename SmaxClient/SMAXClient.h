#pragma once

#include <optional>
#include <boost/beast/http.hpp>
#include <memory>
#include "ConnectionProperties.h"
#include "ResponseHelper.h"

namespace smax_ns {

/**
 * @brief Constant that defines the token life time in minutes.
 */
const short TOKEN_LIFE_TIME_MINUTES = 10;

/**
 * @brief Structure to hold token information including the token string and its creation time.
 */
struct TokenInfo {
    std::string token; ///< The token string
    std::chrono::system_clock::time_point creation_time; ///< The creation time of the token
};

/**
 * @brief A singleton class responsible for interacting with the SMAX system.
 * 
 * The SMAXClient class provides methods for interacting with SMAX APIs, managing tokens, and making HTTP requests.
 */
class SMAXClient {
public:
    /**
     * @brief Get the single instance of the SMAXClient.
     * @param connection_props Connection parameters for establishing the connection.
     * @return SMAXClient& The singleton instance of the SMAXClient.
     */
    static SMAXClient& getInstance(const ConnectionParameters& connection_props);

    /**
     * @brief Deleted copy constructor for SMAXClient.
     */
    SMAXClient(const SMAXClient&) = delete;

    /**
     * @brief Deleted copy assignment operator for SMAXClient.
     * @return SMAXClient& The instance of the class.
     */
    SMAXClient& operator=(const SMAXClient&) = delete;

    /**
     * @brief Deleted move constructor for SMAXClient.
     */
    SMAXClient(SMAXClient&&) = delete;

    /**
     * @brief Deleted move assignment operator for SMAXClient.
     * @return SMAXClient& The instance of the class.
     */
    SMAXClient& operator=(SMAXClient&&) = delete;

    /**
     * @brief Get the authorization URL for the SMAX system.
     * @return std::string The authorization URL.
     */
    std::string getAuthorizationUrl() const;

    /**
     * @brief Get the FRS URL for a specific file ID.
     * @param file_id The file ID.
     * @return std::string The FRS URL for the file.
     */
    std::string getFrsUrl(std::string file_id) const;

    /**
     * @brief Get the EMS URL for a specific layout.
     * @param layout The layout.
     * @return std::string The EMS URL for the layout.
     */
    std::string getEmsUrl(std::string layout) const;

    /**
     * @brief Get the base EMS URL with layout contains set of JSON fields defined by parameters
     * @return std::string The base EMS URL.
     */
    std::string getEmsJsonUrl() const;

    /**
     * @brief Get the base EMS URL.
     * @return std::string The base EMS URL.
     */
    std::string getEmsBaseUrl() const;

    /**
     * @brief Get the bulk post URL for the SMAX system.
     * @return std::string The bulk post URL.
     */
    std::string getBulkPostUrl() const;

    /**
     * @brief Perform a specific action (defined by parameters) in the SMAX system.
     * @return std::string The result of the action.
     */
    std::string doAction();

    /**
     * @brief Get the current authorization token.
     * @return std::string The authorization token.
     */
    std::string getToken();

private:
    const ConnectionParameters& connection_props_; ///< Connection parameters for the client
    static std::unique_ptr<SMAXClient> instance_; ///< The singleton instance of the SMAXClient
    static std::once_flag init_flag_; ///< Flag to ensure initialization occurs only once
    std::optional<TokenInfo> token_info_; ///< Optional token information
    ResponseHelper* response_helper_; ///< Response helper object for processing API responses

    /**
     * @brief Private constructor for initializing the SMAXClient.
     * @param connection_props Connection parameters for the client.
     */
    explicit SMAXClient(const ConnectionParameters& connection_props);

    /**
     * @brief Encode a URL parameter (used for filter).
     * @param value The value to be URL-encoded.
     * @return std::string The URL-encoded value.
     */
    std::string url_encode(const std::string& value) const;

    /**
     * @brief Retrieve data via a GET request.
     * @return std::string The response data.
     */
    std::string getData();

    /**
     * @brief Send data via a POST request.
     * @return std::string The response data.
     */
    std::string postData();

    /**
     * @brief Process and retrieve the attachments data.
     * @return std::string The attachments data.
     */
    std::string processGetAttachments();

    /**
     * @brief Save attachments data to a specified directory.
     * @param data The data to be saved.
     * @return bool True if the attachments were saved successfully, false otherwise.
     */
    bool saveAttachmentsToDirectory(const std::string& data) const;

    /**
     * @brief Save JSON data to a specified directory.
     * @param data The JSON data to be saved.
     * @return bool True if the JSON was saved successfully, false otherwise.
     */
    bool saveJsonToDirectory(const std::string& data) const;

    /**
     * @brief Process the action for JSON data (defined by parameters).
     * @return std::string The processed JSON action result.
     */
    std::string processJsonAction();

    /**
     * @brief Update the token if it is expired or invalid.
     */
    void updateToken();

    /**
     * @brief Get the body for the authentication request.
     * @return std::string The authentication request body.
     */
    std::string getAuthBody() const;

    /**
     * @brief Perform an HTTP request (GET or POST).
     * 
     * @param method The HTTP method (GET, POST, etc.).
     * @param endpoint The request endpoint.
     * @param port The port to use for the request.
     * @param body The request body (for POST requests).
     * @param result The response result.
     * @param headers The request headers.
     * @param status_code The HTTP status code.
     * @return bool True if the request was successful, false otherwise.
     */
    bool perform_request(boost::beast::http::verb method,
        const std::string& endpoint,
        uint16_t port, 
        const std::string& body, std::string& result, 
        const std::map<std::string, std::string>& headers, int& status_code) const;

    /**
     * @brief Perform a POST request for authentication.
     * 
     * @param endpoint The request endpoint.
     * @param port The port to use.
     * @param json_body The JSON body of the request.
     * @param result The response result.
     * @param status_code The HTTP status code.
     * @return bool True if the request was successful, false otherwise.
     */
    bool auth_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result, int& status_code) const;

    /**
     * @brief Perform a POST request for a regular API request.
     * 
     * @param endpoint The request endpoint.
     * @param port The port to use.
     * @param json_body The JSON body of the request.
     * @param result The response result.
     * @param status_code The HTTP status code.
     * @return bool True if the request was successful, false otherwise.
     */
    bool request_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result, int& status_code) const;

    /**
     * @brief Perform a GET request for a regular API request.
     * 
     * @param endpoint The request endpoint.
     * @param port The port to use.
     * @param result The response result.
     * @param status_code The HTTP status code.
     * @return bool True if the request was successful, false otherwise.
     */
    bool request_get(const std::string& endpoint, uint16_t port, std::string& result, int& status_code) const;

    /**
     * @brief Get request information such as endpoint, headers, etc.
     * @return std::string The request information.
     */
    std::string getRequestInfo() const;

    /**
     * @brief Get the base REST URL for the SMAX system.
     * @return std::string The base REST URL.
     */
    std::string getBaseRestUrl() const;

    /**
     * @brief Get the base URL for the SMAX system.
     * @return std::string The base URL.
     */
    std::string getBaseUrl() const;

    /**
     * @brief Parse JSON data from a string.
     * @param data The JSON string to be parsed.
     * @return std::string The parsed data.
     */
    std::string parseJson(const std::string& data);

    /**
     * @brief Send a request to the SMAX system (either GET or POST).
     * 
     * @param endpoint The request endpoint.
     * @param body The request body (for POST requests).
     * @param isPost Whether the request is a POST request.
     * @param result_status_code The HTTP status code.
     * @return std::string The response data.
     */
    std::string sendRequest(const std::string& endpoint, const std::string& body, bool isPost, int & result_status_code);

    /**
     * @brief Get the port number for the SMAX system.
     * @return int The port number.
     */
    int getPort() const;

    /**
     * @brief Save the attachments data to a directory.
     * @param data The attachments data to be saved.
     * @return bool True if the attachments were saved successfully, false otherwise.
     */
    bool doSaveAttachments(const std::string& data) const;
};

} // namespace smax_ns
