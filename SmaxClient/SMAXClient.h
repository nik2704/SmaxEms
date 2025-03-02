#pragma once

#include <optional>
#include <boost/beast/http.hpp>
#include "ConnectionProperties.h"

namespace smax_ns {

const short TOKEN_LIFE_TIME_MINUTES = 10;

struct TokenInfo {
    std::string token;
    std::chrono::system_clock::time_point creation_time;
};

class SMAXClient {
public:
    static SMAXClient& getInstance(const ConnectionParameters& connection_props);

    SMAXClient(const SMAXClient&) = delete;
    SMAXClient& operator=(const SMAXClient&) = delete;
    SMAXClient(SMAXClient&&) = delete;
    SMAXClient& operator=(SMAXClient&&) = delete;

    std::string getAuthorizationUrl() const;
    std::string getEmsUrl() const;
    std::string getBulkPostUrl() const;
    std::string doAction();
    std::string getToken();
private:
    const ConnectionParameters& connection_props_;
    static std::unique_ptr<SMAXClient> instance_;
    static std::once_flag init_flag_;
    std::optional<TokenInfo> token_info_;


    explicit SMAXClient(const ConnectionParameters& connection_props);
    std::string getData();
    std::string postData();
    void updateToken();
    std::string getAuthBody() const;
    bool perform_request(boost::beast::http::verb method,
        const std::string& endpoint,
        uint16_t port, 
        const std::string& body, std::string& result, 
        const std::map<std::string, std::string>& headers, int& status_code) const;

    bool auth_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result, int& status_code) const;
    bool request_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result, int& status_code) const;
    bool request_get(const std::string& endpoint, uint16_t port, std::string& result, int& status_code) const;
    std::string getRequestInfo() const;
    std::string getBaseUrl() const;
    std::string parseJson(const std::string& data);
    std::string sendRequest(const std::string& endpoint, const std::string& body, bool isPost);
    int getPort() const;
};

}