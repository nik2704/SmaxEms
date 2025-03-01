#include <boost/program_options.hpp>
#include <cstdint>
#include <iostream>
#include <fstream>

#include "utils/utils.h"
#include "SmaxClient/SMAXClient.h"
#include "Parser/Parser.h"

namespace po = boost::program_options;
using namespace smax_ns;

int main(int argc, char* argv[]) {
    try {
        smax_ns::InputValues input_values;

        po::options_description desc("Options", 120);
        desc.add_options()
            ("config-file", po::value<std::string>(), "Full name if a config file")
            ("smax-protocol,p", po::value<std::string>(&input_values.protocol)->default_value("https"), "protocol (http | https default)")
            ("smax-host,s", po::value<std::string>(&input_values.host), "FQDN of SMAX server")
            ("smax-port,c", po::value<uint16_t>(&input_values.port)->default_value(80), "Connection port (80 is default)")
            ("smax-secure-port,z", po::value<uint16_t>(&input_values.secure_port)->default_value(443), "HTTPs port (443 is default)")
            ("tenant,t", po::value<std::size_t>(&input_values.tenant), "Tenant ID")
            ("entity,e", po::value<std::string>(&input_values.entity)->default_value("Request"), "Entity name")
            ("layout,l", po::value<std::string>(&input_values.layout)->default_value("Id,DisplayLabel"), "Layout (fields)")
            ("username,U", po::value<std::string>(&input_values.username), "Username")
            ("password,P", po::value<std::string>(&input_values.password), "Password")
            ("filter", po::value<std::string>(&input_values.filter), "Filter (like \"Id='52641'\")")
            ("action", po::value<std::string>(&input_values.action)->required()->default_value("GET"), "Action (GET is default, CREATE, UPDATE)")
            ("csv", po::value<std::string>(&input_values.csv), "CSV file name (required if action is not GET)")
            ("verbose,v", po::bool_switch(&input_values.verbose)->default_value(false), "Enable verbose output")
            ("help,h", "Help");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("config-file")) {
            std::ifstream config_file(vm["config-file"].as<std::string>());
            if (config_file) {
                po::store(po::parse_config_file(config_file, desc), vm);
            } else {
                std::cerr << "ERROR: impossible to open the config file.\n";
                return 1;
            }
        }

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        po::notify(vm);

        auto validation_result = validate_input_values(input_values);
        if (validation_result->result != 0) {
            std::cout << "ERROR: " << validation_result->message << "\n";
            return 1;
        }

        smax_ns::ConnectionParameters& conn_params = smax_ns::ConnectionParameters::getInstance(input_values);
        smax_ns::SMAXClient& smax_client = smax_ns::SMAXClient::getInstance(conn_params);

        std::cout << smax_client.doAction() << std::endl;

        smax_ns::Parser parser(input_values.csv);
        parser.parseCSV(input_values.entity, input_values.action);

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
