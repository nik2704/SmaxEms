#include <boost/program_options.hpp>
#include <cstdint>
#include <iostream>
#include <fstream>

#include "utils/utils.h"
#include "SmaxClient/SMAXClient.h"
#include "Parser/Parser.h"

using namespace smax_ns;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    try {
        smax_ns::InputValues input_values;
        po::variables_map vm;

        if (!parse_options(argc, argv, input_values, vm)) {
            return 1;
        }

        auto validation_result = validate_input_values(input_values);
        if (validation_result->result != 0) {
            std::cerr << "ERROR: " << validation_result->message << "\n";
            return 1;
        }

        smax_ns::ConnectionParameters& conn_params = smax_ns::ConnectionParameters::getInstance(input_values);
        smax_ns::SMAXClient& smax_client = smax_ns::SMAXClient::getInstance(conn_params);

        auto result = smax_client.doAction();

        std::cout << "**************Response:********************\n";
        std::cout << result << "\n";

        if (!input_values.csv.empty()) {
            smax_ns::Parser parser(input_values.csv);
            parser.parseCSV(input_values.entity, input_values.action);
        }

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
