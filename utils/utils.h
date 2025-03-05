#pragma once

#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include "../SmaxClient/ConnectionProperties.h"


namespace po = boost::program_options;

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

std::unique_ptr<ValidationResult> validate_attachments_actions(const InputValues& input);

std::unique_ptr<ValidationResult> validate_input_values(const InputValues& input);

bool parse_options(int argc, char* argv[], smax_ns::InputValues& input_values, po::variables_map& vm);

} // namespace smax_ns
