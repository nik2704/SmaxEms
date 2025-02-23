#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class RestClient : public std::enable_shared_from_this<RestClient> {
public:
    using ResponseHandler = std::function<void(const std::string&, const beast::error_code&)>;

    RestClient(net::io_context& ioc, const std::string& host, const std::string& port);

    void run(const std::string& target, http::verb method = http::verb::get, 
             const std::string& body = "", ResponseHandler handler = nullptr);

private:
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
    http::request<http::string_body> req_;
    http::response<http::dynamic_body> res_;
    std::string host_, port_, target_;
    ResponseHandler response_handler_;
    beast::flat_buffer buffer_;

    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void fail(beast::error_code ec, const char* what);
};
