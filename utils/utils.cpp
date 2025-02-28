#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <future>
#include <iostream>

#include "../client/RestClient.h"
#include "../client/ConnectionProperties.h"
#include "utils.h"

namespace smax_ns {

std::unique_ptr<InputValues> create_default_input_values() {
    return std::make_unique<InputValues>(InputValues {
        "https", "", 80, 443, 0, "", "Id,DisplayLabel", "", "", ""
    });
};

bool is_string_equals(const std::string& a, const std::string& b) {
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(), [](char a, char b) {
               return std::tolower(a) == std::tolower(b);
           });
}

std::unique_ptr<ValidationResult> validate_port(const InputValues& input) {
    if (input.port == 0) {
        return std::make_unique<ValidationResult>(ValidationResult{
            "Port should be in the diapazon [1, 65535]", 1
        });
    }

#include <iostream>
    return std::make_unique<ValidationResult>(ValidationResult{"", 0});
}

std::unique_ptr<ValidationResult> validate_protocol(const InputValues& input) {
    if (!is_string_equals(input.protocol, "http") && !is_string_equals(input.protocol, "https")) {
        return std::make_unique<ValidationResult>(ValidationResult{
            "Unsupported protocol. Acceptable values are: http, https.", 1
        });
    }
    
    return std::make_unique<ValidationResult>(ValidationResult{"", 0});
}

std::unique_ptr<ValidationResult> validate_input_values(const InputValues& input) {
    if (input.host.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Host should not be EMPTY.", 1});
    }

    if (input.host.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Entity should not be EMPTY.", 1});
    }

    if (input.tenant < 10000000) {
        return std::make_unique<ValidationResult>(ValidationResult{"Tenant ID should be grater than 10000000.", 1});
    }

    if (input.layout.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Layout should not be EMPTY.", 1});
    }

    if (input.username.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Username should not be EMPTY.", 1});
    }

    if (input.password.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Password should not be EMPTY.", 1});
    }

    auto output_result = validate_port(input);
    if (output_result->result != 0) return output_result;

    auto format_result = validate_protocol(input);
    if (format_result->result != 0) return format_result;

    return std::make_unique<ValidationResult>(ValidationResult{"Paremeters are correct.", 0});
}

}