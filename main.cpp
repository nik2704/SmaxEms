#include <boost/program_options.hpp>
#include <cstdint>
#include <iostream>
#include <fstream>

#include "utils/utils.h"
#include "client/ConnectionProperties.h"

namespace po = boost::program_options;

using namespace smax_ns;

int main(int argc, char* argv[]) {
    try {
        auto input_values = create_default_input_values();

        po::options_description desc("Допустимые опции", 120);
        desc.add_options()
            ("smax-protocol,p", po::value<std::string>(&input_values->protocol)->default_value("https"), "protocol (http | https default)")
            ("smax-host,s", po::value<std::string>(&input_values->host), "FQDN of SMAX server")
            ("smax-port,P", po::value<uint16_t>(&input_values->port)->default_value(80), "Connection port (80 is default)")
            ("tenant,t", po::value<std::size_t>(&input_values->tenant), "Tenant ID")
            ("entity,e", po::value<std::string>(&input_values->entity)->default_value("Request"), "Entity name")
            ("layout,l", po::value<std::string>(&input_values->layout)->default_value("Id,DisplayLabel"), "Layout (fields)")
            ("help,h", "Помощь");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        po::notify(vm);

        auto validation_result = validate_input_values(*input_values);

        if (validation_result->result != 0) {
            std::cout << "Ошибка: " << validation_result->message << "\n";
            return 1;
        }

        // Обрабатываем файл согласно введенным параметрам
        // process(*input_values);
        smax_ns::ConnectionParameters& conn_params = smax_ns::ConnectionParameters::GetInstance(*input_values);
        std::cout << conn_params << std::endl;

        std::cout << input_values->protocol << "://" << input_values->host << ":" << input_values->port << "/rest/" << input_values->entity << std::endl;

        std::string response = perform_request("jsonplaceholder.typicode.com", "80");

        std::cout << "Ответ от сервера:\n" << response << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
