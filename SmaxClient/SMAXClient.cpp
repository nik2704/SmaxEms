#include <boost/asio.hpp>
#include <chrono>
#include <future>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../RestClient/RestClient.h"
#include "SMAXClient.h"

using json = nlohmann::json;

namespace smax_ns {

std::unique_ptr<SMAXClient> SMAXClient::instance_ = nullptr;
std::once_flag SMAXClient::init_flag_;

SMAXClient& SMAXClient::getInstance(const ConnectionParameters& connection_props) {
    std::call_once(init_flag_, [&]() {
        instance_.reset(new SMAXClient(connection_props));
    });
    return *instance_;
}

SMAXClient::SMAXClient(const ConnectionParameters& connection_props) : connection_props_(connection_props) {}

std::string SMAXClient::getAuthorizationUrl() const {
    std::ostringstream url;
    url << connection_props_.getProtocol() << "://" << connection_props_.getHost();
    uint16_t port = connection_props_.getSecurePort();
    if (port != 80) {
        url << ":" << port;
    }
    url << "/auth/authentication-endpoint/authenticate/login?TENANTID=" << connection_props_.getTenant();
    return url.str();
}

std::string SMAXClient::getEmsUrl() const {
    std::ostringstream url;
    url << connection_props_.getProtocol() << "://" << connection_props_.getHost();
    uint16_t port = connection_props_.getPort();
    
    if (port != 80) {
        url << ":" << port;
    }
    
    url << "/rest/" << connection_props_.getTenant() << "/ems/" << connection_props_.getEntity() << "?layout=";
    url << connection_props_.getLayout();

    if (!connection_props_.getFilter().empty()) {
        url << "&filter=" << connection_props_.getFilter();
    }

    return url.str();
}

std::string SMAXClient::doAction() {
    auto action = connection_props_.getAction();

    if (connection_props_.isVerbose()) {
        return "Verbose";
    }

    if (action == "GET") {
        return getData();
    }

    if (action == "CREATE") {
        return getData();
    }

    if (action == "UPDATE") {
        return getData();
    }

    return "Unsupported action";
}

std::string SMAXClient::getData() {
    std::string result = "ERROR";
    updateToken();

    if (token_info_.token != "ERROR") {
        std::string endpoint = getEmsUrl();
        auto port = connection_props_.getProtocol() == "http" ? connection_props_.getPort() : connection_props_.getSecurePort();
        int status_code;

        request_get(endpoint, port, result, status_code);

        if (status_code != 200) {
            std::cerr << "Ошибка получения данных" << std::endl;
            return result;
        }

        try {
            json parsed_json = json::parse(result);
            return parsed_json.dump(4);
        } catch (const std::exception& e) {
            std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
        }        
    }
    
    return result;
}

void SMAXClient::updateToken() {
    auto now = std::chrono::system_clock::now();
    auto token_age = std::chrono::duration_cast<std::chrono::minutes>(now - token_info_.creation_time).count();

    if (token_info_.token.empty() || token_age > TOKEN_LIFE_TIME_MINUTES) {
        if (getToken() == "ERROR") {
            std::cerr << "Ошибка получения токена" << std::endl;
        }
    }
}


std::string SMAXClient::getToken() {
    std::ostringstream json_stream;
    json_stream << R"({"login":")" << connection_props_.getUserName() << R"(", "password":")" << connection_props_.getPassword() << R"("})";

    std::string json_body = json_stream.str();
    auto endpoint = getAuthorizationUrl();
    auto port = connection_props_.getSecurePort();

    std::string token;
    int status_code;

    bool success = request_post(endpoint, port, json_body, token, status_code);
    if (status_code != 200) {
        success = false;
    }

    if (success) {
        token_info_.token = token;
        token_info_.creation_time = std::chrono::system_clock::now();
    } else {
        token_info_.token = "ERROR";
    }

    return token_info_.token;
}

bool SMAXClient::perform_request(http::verb method, const std::string& endpoint, uint16_t port,
                                 const std::string& body, std::string& result,
                                 const std::map<std::string, std::string>& headers, int& status_code) const {
    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx(boost::asio::ssl::context::tls_client);
    std::promise<std::tuple<bool, std::string, int>> promise;
    auto future = promise.get_future();
    auto host = connection_props_.getHost();

    try {
        auto client = std::make_shared<RestClient>(ioc, ctx, host, std::to_string(port));

        client->run(endpoint, method, body, 
            [&promise](const std::string& response, const boost::system::error_code& ec, int http_status) {
                if (!ec) {
                    promise.set_value({true, response, http_status});
                } else {
                    std::cerr << "Error: " << ec.message() << "\n";
                    promise.set_value({false, "Ошибка запроса: " + ec.message(), http_status});
                }
            }, headers);

        ioc.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        promise.set_value({false, "Исключение: " + std::string(e.what()), 0});
    }

    auto [success, response, http_status] = future.get();
    result = response;
    status_code = http_status;
    
    return success;
}

bool SMAXClient::request_get(const std::string& endpoint, uint16_t port, std::string& result, int& status_code) const {
    return perform_request(http::verb::get, endpoint, port, "", result, {{"Cookie", "SMAX_AUTH_TOKEN=" + token_info_.token}}, status_code);
}

bool SMAXClient::request_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result, int& status_code) const {
    return perform_request(http::verb::post, endpoint, port, json_body, result, {}, status_code);
}


}
