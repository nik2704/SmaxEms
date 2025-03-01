#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <future>
#include <iostream>

#include "../SmaxClient/ConnectionProperties.h"
#include "utils.h"

namespace smax_ns {

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

std::unique_ptr<ValidationResult> validate_action(const InputValues& input) {
    if (input.action == "GET" || input.action == "CREATE" ||input.action == "UPDATE") {
        return std::make_unique<ValidationResult>(ValidationResult{"", 0});
    }

    return std::make_unique<ValidationResult>(ValidationResult{"Action should be GET | UPDATE | CREATE", 1});
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

    output_result = validate_protocol(input);
    if (output_result->result != 0) return output_result;

    output_result = validate_action(input);
    if (output_result->result != 0) return output_result;

    if (input.action != "GET" && input.csv.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"CSV is mandatory for CREATE or UPDATE", 1});
    }

    return std::make_unique<ValidationResult>(ValidationResult{"Paremeters are correct.", 0});
}

}