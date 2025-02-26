#pragma once

#include <memory>
#include <mutex>
#include "ConnectionProperties.h"

namespace smax_ns {

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
    std::string getToken();
private:
    const ConnectionParameters& connection_props_;
    static std::unique_ptr<SMAXClient> instance_;
    static std::once_flag init_flag_;
    TokenInfo token_info_;

    explicit SMAXClient(const ConnectionParameters& connection_props);
    bool request_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result) const;
};

}
