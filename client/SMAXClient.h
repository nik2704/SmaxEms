#pragma once

#include <memory>
#include <mutex>
#include "ConnectionProperties.h"

namespace smax_ns {

class SMAXClient {
public:
    static SMAXClient& getInstance(const ConnectionParameters& connection_props);

    SMAXClient(const SMAXClient&) = delete;
    SMAXClient& operator=(const SMAXClient&) = delete;
    SMAXClient(SMAXClient&&) = delete;
    SMAXClient& operator=(SMAXClient&&) = delete;

    std::string getAuthorizationUrl() const;
    std::string getEmsUrl() const;

private:
    const ConnectionParameters& connection_props_;
    static std::unique_ptr<SMAXClient> instance_;
    static std::once_flag init_flag_;

    explicit SMAXClient(const ConnectionParameters& connection_props);
};

}
