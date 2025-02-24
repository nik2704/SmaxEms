#pragma once

#include <string>
#include <iostream>
#include <cctype>
#include <mutex>

namespace smax_ns {

inline std::string ToLower(const std::string& input);

struct InputValues {
    std::string protocol;           ///< Protocol
    std::string host;               ///< SMAX host
    uint16_t port;                  ///< SMAX port
    std::size_t tenant;             ///< Tenant ID
    std::string entity;             ///< Entity Name
    std::string layout;             ///< Layout (fields)
    std::string username;           ///< User name
    std::string password;           ///< Password
};

class ConnectionParameters {
public:
    static ConnectionParameters& GetInstance(const InputValues& input_values);

    static ConnectionParameters& GetInstance();

    const std::string& GetProtocol() const;
    const std::string& GetHost() const;
    uint16_t GetPort() const;
    std::size_t GetTenant() const;
    const std::string& GetEntity() const;
    const std::string& GetLayout() const;

    friend std::ostream& operator<<(std::ostream& os, const ConnectionParameters& params) {
        os << "Protocol: " << params.protocol_ << "\n"
           << "Host: " << params.host_ << "\n"
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

    static ConnectionParameters* instance_;

    std::string protocol_;
    std::string host_;
    uint16_t port_;
    std::size_t tenant_;
    std::string entity_;
    std::string layout_;
};

} // namespace smax_ems
