#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <future>
#include <iostream>

#include "../client/RestClient.h"
#include "utils.h"

namespace smax_ems {

std::unique_ptr<InputValues> create_default_input_values() {
    return std::make_unique<InputValues>(InputValues {
        "https", "", 80, 0, "", "Id,DisplayLabel", "", ""
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

    auto output_result = validate_port(input);
    if (output_result->result != 0) return output_result;

    auto format_result = validate_protocol(input);
    if (format_result->result != 0) return format_result;

    return std::make_unique<ValidationResult>(ValidationResult{"Paremeters are correct.", 0});
}

std::string perform_request(const std::string& host, const std::string& port) {
    boost::asio::io_context ioc;
    std::promise<std::string> promise;
    auto future = promise.get_future();

    try {
        auto client = std::make_shared<RestClient>(ioc, host, port);

        client->run("/posts/1", http::verb::get, "", 
            [&promise](const std::string& response, const boost::system::error_code& ec) {
                if (!ec) {
                    promise.set_value(response);  // Устанавливаем результат
                } else {
                    promise.set_value("Ошибка GET: " + ec.message());  // В случае ошибки
                }
            });

        ioc.run();  // Запускаем io_context для обработки асинхронных операций

    } catch (const std::exception& e) {
        promise.set_value(std::string("Исключение: ") + e.what());
    }

    return future.get();  // Ждём завершения асинхронного запроса
}


}