#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "../SmaxClient/ConnectionProperties.h"

namespace smax_ns {

struct ValidationResult {
    std::string message;
    int result;
};

std::unique_ptr<InputValues> create_default_input_values();

bool is_string_equals(const std::string& a, const std::string& b);

std::unique_ptr<ValidationResult> validate_port(const InputValues& input);

std::unique_ptr<ValidationResult> validate_protocol(const InputValues& input);

std::unique_ptr<ValidationResult> validate_json_actions(const InputValues& input);

std::unique_ptr<ValidationResult> validate_input_values(const InputValues& input);

} // namespace smax_ns
