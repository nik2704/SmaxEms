#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace smax_ems {

struct InputValues {
    std::string protocol;           ///< protocol
    std::string host;               ///< SMAX host
    uint16_t port;                  ///< SMAX port
    std::size_t tenant;             ///< Tenant ID
    std::string entity;             ///< Entity Name
    std::string layout;             ///< Layuot (fields)
    std::string username;           ///< User name
    std::string passord;            ///< password
};

struct ValidationResult {
    std::string message;
    int result;
};

std::unique_ptr<InputValues> create_default_input_values();

bool is_string_equals(const std::string& a, const std::string& b);

std::unique_ptr<ValidationResult> validate_port(const InputValues& input);

std::unique_ptr<ValidationResult> validate_protocol(const InputValues& input);

std::unique_ptr<ValidationResult> validate_input_values(const InputValues& input);

std::string perform_request(const std::string& host, const std::string& port);

} // namespace smax_ems
