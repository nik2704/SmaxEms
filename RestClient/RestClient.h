#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <functional>
#include <string>
#include <map>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

/**
 * @class RestClient
 * @brief Asynchronous REST client using Boost.Beast and Boost.Asio for HTTPS communication.
 */
class RestClient : public std::enable_shared_from_this<RestClient> {
public:
    /**
     * @brief Callback type for handling HTTP responses.
     * @param response_body The body of the HTTP response.
     * @param error_code The error code if an error occurred.
     * @param status_code The HTTP status code of the response.
     */
    using ResponseHandler = std::function<void(const std::string&, const boost::system::error_code&, int)>;

    /**
     * @brief Constructs a RestClient instance.
     * @param ioc The Boost.Asio I/O context.
     * @param ctx The SSL context.
     * @param host The target host.
     * @param port The target port.
     */
    RestClient(net::io_context& ioc, ssl::context& ctx, const std::string& host, const std::string& port);

    /**
     * @brief Initiates an asynchronous HTTP request.
     * @param target The target path on the server.
     * @param method The HTTP method (GET, POST, etc.).
     * @param body The request body (if applicable).
     * @param handler The callback function to handle the response.
     * @param headers Additional headers to include in the request.
     */
    void run(const std::string& target, http::verb method = http::verb::get, 
             const std::string& body = "", ResponseHandler handler = nullptr,
             const std::map<std::string, std::string>& headers = {});

private:
    tcp::resolver resolver_;  ///< Resolves the target host and port.
    beast::ssl_stream<beast::tcp_stream> stream_;  ///< Secure SSL stream.
    http::request<http::string_body> req_;  ///< HTTP request object.
    http::response<http::dynamic_body> res_;  ///< HTTP response object.
    std::string host_, port_, target_;  ///< Connection parameters.
    ResponseHandler response_handler_;  ///< Callback handler for response processing.
    beast::flat_buffer buffer_;  ///< Buffer for storing received data.

    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    void on_handshake(beast::error_code ec);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void fail(beast::error_code ec, const char* what);
};
