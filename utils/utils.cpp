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
    if (input.action == "GET" || input.action == "CREATE" || input.action == "UPDATE" || input.action == "JSON") {
        return std::make_unique<ValidationResult>(ValidationResult{"", 0});
    }

    return std::make_unique<ValidationResult>(ValidationResult{"Action should be GET | UPDATE | CREATE | JSON", 1});
}

std::unique_ptr<ValidationResult> validate_custom_actions(const InputValues& input) {
    if (input.custom_action_file.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Custom action file should not be EMPTY.", 1});
    }

    if (input.custom_action_field.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Custom action field should not be EMPTY.", 1});
    }
    
    if (!is_string_equals(input.custom_action_output, "console") && !is_string_equals(input.custom_action_output, "file")) {
        return std::make_unique<ValidationResult>(ValidationResult{ "Acceptable action's output values are: http, https.", 1 });
    }

    if (is_string_equals(input.custom_action_output, "file")) {
        if (input.custom_action_output_folder.empty()) {
            return std::make_unique<ValidationResult>(ValidationResult{ "Output folder should not be empty", 1 });
        }
    }

    if (is_string_equals(input.custom_action, "COPYJSON")) {
        if (input.custom_action_src_id.empty()) {
            return std::make_unique<ValidationResult>(ValidationResult{"SOURCE ID should not be EMPTY.", 1});
        }

        if (input.custom_action_tgt_id.empty()) {
            return std::make_unique<ValidationResult>(ValidationResult{"TARGET ID should not be EMPTY.", 1});
        }

        if (is_string_equals(input.custom_action_src_id, input.custom_action_tgt_id)) {
            return std::make_unique<ValidationResult>(ValidationResult{"SORCE ID should not be the same as TARGET ID.", 1});
        }

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

    output_result = validate_protocol(input);
    if (output_result->result != 0) return output_result;

    output_result = validate_action(input);
    if (output_result->result != 0) return output_result;

    if (input.action != "GET" && input.csv.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"CSV is mandatory for CREATE or UPDATE", 1});
    }

    if (input.action == "CUSTOM") {
        output_result = validate_custom_actions(input);

        if (output_result->result != 0) return output_result;
    }

    return std::make_unique<ValidationResult>(ValidationResult{"Paremeters are correct.", 0});
}

}