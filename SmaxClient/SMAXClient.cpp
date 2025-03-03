#include <boost/asio.hpp>
#include <chrono>
#include <future>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../RestClient/RestClient.h"
#include "../Parser/Parser.h"
#include "../utils/utils.h"
#include "ConsoleSpinner.h"
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
    return connection_props_.getProtocol() + "://" + connection_props_.getHost() +
           (connection_props_.getSecurePort() != 80 ? ":" + std::to_string(connection_props_.getSecurePort()) : "") +
           "/auth/authentication-endpoint/authenticate/login?TENANTID=" + std::to_string(connection_props_.getTenant());
}

std::string SMAXClient::getBaseUrl() const {
    std::ostringstream url;
    url << connection_props_.getProtocol() << "://" << connection_props_.getHost();
    
    uint16_t port = connection_props_.getPort();
    if (port != 80) {
        url << ":" << port;
    }

    url << "/rest/" << connection_props_.getTenant() << "/ems";
    return url.str();
}

std::string SMAXClient::getEmsUrl() const {
    std::ostringstream url;
    url << getBaseUrl() << "/" << connection_props_.getEntity() << "?layout=" << connection_props_.getLayout();
    
    if (!connection_props_.getFilter().empty()) {
        url << "&filter=" << connection_props_.getFilter();
    }

    return url.str();
}

std::string SMAXClient::getBulkPostUrl() const {
    std::ostringstream url;
    url << getBaseUrl() << "/bulk";
    return url.str();
}

std::string SMAXClient::doAction() {
    if (connection_props_.isVerbose()) return getRequestInfo();

    switch (connection_props_.getAction()) {
    case Action::GET:
        return getData();

    case Action::CREATE:
        return postData();

    case Action::UPDATE:
        return postData();

    case Action::CUSTOM:
        return "CUSTOM";

    default:
        break;
    }
    
    return "Unsupported action";
}

std::string SMAXClient::getRequestInfo() const {
    std::ostringstream oss;
    std::string http_action = "POST";
    json authBody = json::parse(getAuthBody());
    bool parsePostBody = true;

    oss << "Request parameters:\n"
        << "1) Authorization URL: " << getAuthorizationUrl() << "\n"
        << "Authorization body:\n"
        << authBody.dump(4) << "\n\n";

    auto action = connection_props_.getAction();

    switch (action) {
    case smax_ns::Action::GET :
        oss << "2) URL: " << getEmsUrl() << "\n";
        http_action = "GET";
        parsePostBody= false;

        break;
    case smax_ns::Action::CUSTOM :
        if (is_string_equals(connection_props_.getCustomAction(), "GETJSON")) {
            oss << "2) URL: " << getEmsUrl() << "\n";
            http_action = "GET";
        } else if (is_string_equals(connection_props_.getCustomAction(), "COPYJSON")) {
            oss << "2) URLs: " << "\n"
                << getEmsUrl() << "\n"
                << getBulkPostUrl() << "\n";
            http_action = "GET & POST";
        }

        break;
    default:
        oss << "2) URL: " << getBulkPostUrl() << "\n";

        break;
    }

    oss << "3) Action: " << connection_props_.getActionAsString() << "\n"
        << "4) HTTP action: <" << http_action << ">\n";

    if (parsePostBody) {
        if (action == smax_ns::Action::CREATE || action == smax_ns::Action::UPDATE) {
            Parser parser(connection_props_.getCSVfilename());

            auto postBody = parser.parseCSV(connection_props_.getEntity(), "POST");
            oss << "5) POST Body:\n"
            << postBody.dump(4) << "\n";
        } else {
            oss << "5) POST Body will be defined using the SOURCE Record\n";
        }
    }        

    return oss.str();
}

std::string SMAXClient::postData() {
    int status_code;
    Parser parser(connection_props_.getCSVfilename());
    auto postBody = parser.parseCSV(connection_props_.getEntity(), connection_props_.getActionAsString()).dump();

    return sendRequest(getBulkPostUrl(), postBody, true, status_code);
}

std::string SMAXClient::getData() {
    int status_code;
    auto result = sendRequest(getEmsUrl(), "", false, status_code);
    return result;
}

std::string SMAXClient::sendRequest(const std::string& endpoint, const std::string& body, bool isPost, int & result_status_code) {
    updateToken();

    std::ostringstream oss;
    oss << "Sending " << connection_props_.getActionAsString() << " request ";
    ConsoleSpinner spinner(oss.str());

    if (!token_info_.has_value() || token_info_->token == "ERROR") {
        return "ERROR";
    }

    std::future<std::string> future = std::async(std::launch::async, [&] {
        std::string result;
        int status_code;

        bool success = isPost ? request_post(endpoint, getPort(), body, result, status_code)
                              : request_get(endpoint, getPort(), result, status_code);

        result_status_code = status_code;
        spinner.setStatus(std::to_string(status_code));

        return (success && status_code == 200) ? parseJson(result) : "ERROR";
    });

    return future.get();
}


std::string SMAXClient::parseJson(const std::string& data) {
    try {
        json parsed_json = json::parse(data);
        return parsed_json.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
        return "ERROR";
    }
}

int SMAXClient::getPort() const {
    return (connection_props_.getProtocol() == "http") ? 
            connection_props_.getPort() : connection_props_.getSecurePort();
}

void SMAXClient::updateToken() {
    auto now = std::chrono::system_clock::now();

    if (!token_info_.has_value() || 
        std::chrono::duration_cast<std::chrono::minutes>(now - token_info_->creation_time).count() > TOKEN_LIFE_TIME_MINUTES) {
        
        if (getToken() == "ERROR") {
            std::cerr << "Ошибка получения токена" << std::endl;
        }
    }
}

std::string SMAXClient::getAuthBody() const {
    std::ostringstream json_stream;
    json_stream << R"({"login":")" << connection_props_.getUserName() << R"(", "password":")" << connection_props_.getPassword() << R"("})";

    return json_stream.str();
}

std::string SMAXClient::getToken() {
    ConsoleSpinner spinner("Getting a new token");

    std::string json_body = getAuthBody();
    auto endpoint = getAuthorizationUrl();
    auto port = connection_props_.getSecurePort();

    std::string token;
    int status_code;

    bool success = auth_post(endpoint, port, json_body, token, status_code);

    spinner.setStatus(std::to_string(status_code));

    if (!success || status_code != 200) {
        token_info_.reset();
        return "ERROR";
    }

    token_info_ = TokenInfo{token, std::chrono::system_clock::now()};

    return token;
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
    return perform_request(http::verb::get, endpoint, port, "", result, {{"Cookie", "SMAX_AUTH_TOKEN=" + token_info_->token}}, status_code);
}

bool SMAXClient::auth_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result, int& status_code) const {
    return perform_request(http::verb::post, endpoint, port, json_body, result, {}, status_code);
}

bool SMAXClient::request_post(const std::string& endpoint, uint16_t port, const std::string& json_body, std::string& result, int& status_code) const {
    return perform_request(http::verb::post, endpoint, port, json_body, result, {{"Cookie", "SMAX_AUTH_TOKEN=" + token_info_->token}}, status_code);
}


}