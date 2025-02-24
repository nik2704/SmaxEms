#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "../client/ConnectionProperties.h"

namespace smax_ems {

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
