#include <boost/asio.hpp>
#include <chrono>
#include <future>
#include <sstream>

#include "RestClient.h"
#include "SMAXClient.h"

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
//&size=3&skip=3&meta=totalCount,Count


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

std::string SMAXClient::getToken() {
    std::ostringstream json_stream;
    json_stream << R"({"login":")" << connection_props_.getUserName() << R"(", "password":")" << connection_props_.getPassword() << R"("})";

    std::string json_body = json_stream.str();
    auto endpoint = getAuthorizationUrl();
    auto port = connection_props_.getSecurePort();

    std::string token;

    bool success = request_post(endpoint, port, json_body, token);

    if (success) {
        token_info_.token = token;
        token_info_.creation_time = std::chrono::system_clock::now();
    }

    return success ? token : "ERROR";
}

bool SMAXClient::request_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result) const {
    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx(boost::asio::ssl::context::tls_client);
    std::promise<std::pair<bool, std::string>> promise;
    auto future = promise.get_future();

    auto host = connection_props_.getHost();

    try {
        auto client = std::make_shared<RestClient>(ioc, ctx, host, std::to_string(port));

        client->run(endpoint, http::verb::post, json_body, 
            [&promise](const std::string& response, const boost::system::error_code& ec) {
                if (!ec) {
                    promise.set_value({true, response});
                } else {
                    std::cerr << "Setting promise value with error: " << ec.message() << "\n";
                    promise.set_value({false, "Ошибка POST: " + ec.message()});
                }
            });

        ioc.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << "\n";
        promise.set_value({false, "Исключение: " + std::string(e.what())});
    }

    auto [success, response] = future.get();
    result = response;
    return success;
}


}
