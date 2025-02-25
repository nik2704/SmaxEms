#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <mutex>

namespace smax_ns {

inline std::string ToLower(const std::string& input);

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
};

} // namespace smax_ns