#include <fstream>
#include <iostream>
#include <string>

#include "../SmaxClient/ConnectionProperties.h"
#include "utils.h"

namespace po = boost::program_options;

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
    if (input.action == "GET" || input.action == "CREATE" || input.action == "UPDATE" || input.action == "JSON" || input.action == "GETATTACHMENTS") {
        return std::make_unique<ValidationResult>(ValidationResult{"", 0});
    }

    return std::make_unique<ValidationResult>(ValidationResult{"Action should be GET | UPDATE | CREATE | JSON | GETATTACHMENTS", 1});
}

std::unique_ptr<ValidationResult> validate_json_actions(const InputValues& input) {
    if (input.json_action_field.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Json action field should not be EMPTY.", 1});
    }
    
    if (!is_string_equals(input.json_action_output, "console") && !is_string_equals(input.json_action_output, "file")) {
        return std::make_unique<ValidationResult>(ValidationResult{ "Acceptable action's output values are: file, console.", 1 });
    }

    if (is_string_equals(input.json_action_output, "file")) {
        if (input.json_action_output_folder.empty()) {
            return std::make_unique<ValidationResult>(ValidationResult{ "Output folder should not be empty", 1 });
        }
    }

    return std::make_unique<ValidationResult>(ValidationResult{"", 0});
}

std::unique_ptr<ValidationResult> validate_attachments_actions(const InputValues& input) {
    if (input.att_action_field.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Attachment field should not be EMPTY.", 1});
    }

    if (input.att_action_output_folder.empty()) {
        return std::make_unique<ValidationResult>(ValidationResult{"Attachment folder parameter should not be EMPTY.", 1});
    }

    if (!is_string_equals(input.att_action_output, "console") && !is_string_equals(input.att_action_output, "file")) {
        return std::make_unique<ValidationResult>(ValidationResult{ "Acceptable action's output values are: file, console.", 1 });
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

    if (input.action == "JSON") {
        output_result = validate_json_actions(input);

        if (output_result->result != 0) return output_result;
    }

    if (input.action == "GETATTACHMENTS") {
        output_result = validate_attachments_actions(input);

        if (output_result->result != 0) return output_result;
    }

    return std::make_unique<ValidationResult>(ValidationResult{"Paremeters are correct.", 0});
}

bool parse_options(int argc, char* argv[], smax_ns::InputValues& input_values, po::variables_map& vm) {
    po::options_description desc("Options", 120);
    desc.add_options()
        ("action", po::value<std::string>(&input_values.action)->required()->default_value("GET"), 
         "Action (GET is default, CREATE, UPDATE, JSON)")
        ("config-file", po::value<std::string>(), "Full name of a config file")
        ("csv", po::value<std::string>(&input_values.csv), "CSV file name (required if action is not GET)")
        ("entity,e", po::value<std::string>(&input_values.entity)->default_value("Request"), "Entity name")
        ("filter", po::value<std::string>(&input_values.filter), "Filter (like \"Id='52641'\")")
        ("layout,l", po::value<std::string>(&input_values.layout)->default_value("Id,DisplayLabel"), "Layout (fields)")
        ("password,P", po::value<std::string>(&input_values.password), "Password")
        ("smax-protocol,p", po::value<std::string>(&input_values.protocol)->default_value("https"), "Protocol (http | https default)")
        ("smax-host,s", po::value<std::string>(&input_values.host), "FQDN of SMAX server")
        ("smax-port,c", po::value<uint16_t>(&input_values.port)->default_value(80), "Connection port (80 is default)")
        ("smax-secure-port,z", po::value<uint16_t>(&input_values.secure_port)->default_value(443), "HTTPS port (443 is default)")
        ("tenant,t", po::value<std::size_t>(&input_values.tenant), "Tenant ID")
        ("username,U", po::value<std::string>(&input_values.username), "Username")
        ("verbose,v", po::bool_switch(&input_values.verbose)->default_value(false), "Enable verbose output")
        ("json-action-field", po::value<std::string>(&input_values.json_action_field), "Json action field")
        ("json-action-output", po::value<std::string>(&input_values.json_action_output)->default_value("console"), "Json action output")
        ("json-action-output-folder", po::value<std::string>(&input_values.json_action_output_folder), "Json action output folder")
        ("att-action-output", po::value<std::string>(&input_values.att_action_output)->default_value("console"), "Json action output")
        ("att_action_field", po::value<std::string>(&input_values.att_action_field), "Field with attachments")
        ("att-action-output-folder", po::value<std::string>(&input_values.att_action_output_folder), "Attachments action output folder")
        ("help,h", "Help");

    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("config-file")) {
        std::ifstream config_file(vm["config-file"].as<std::string>());
        if (config_file) {
            po::store(po::parse_config_file(config_file, desc), vm);
        } else {
            std::cerr << "ERROR: impossible to open the config file.\n";
            return false;
        }
    }

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return false;
    }

    po::notify(vm);
    return true;
}

}