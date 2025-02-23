#include "rest_client.h"
#include <iostream>

RestClient::RestClient(net::io_context& ioc, const std::string& host, const std::string& port)
    : resolver_(ioc), stream_(ioc), host_(host), port_(port) {}

void RestClient::run(const std::string& target, http::verb method, const std::string& body, ResponseHandler handler) {
    target_ = target;
    response_handler_ = handler;

    req_.version(11);  // HTTP/1.1
    req_.method(method);
    req_.target(target_);
    req_.set(http::field::host, host_);
    req_.set(http::field::user_agent, "RestClient/1.0");

    if (method == http::verb::post) {
        req_.set(http::field::content_type, "application/json");
        req_.body() = body;
        req_.prepare_payload();
    }

    resolver_.async_resolve(host_, port_,
        std::bind(&RestClient::on_resolve, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
}

void RestClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec) return fail(ec, "resolve");

    stream_.async_connect(results,
        std::bind(&RestClient::on_connect, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
}

void RestClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
    if (ec) return fail(ec, "connect");

    http::async_write(stream_, req_,
        std::bind(&RestClient::on_write, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
}

void RestClient::on_write(beast::error_code ec, std::size_t bytes_transferred) {
    if (ec) return fail(ec, "write");
    boost::ignore_unused(bytes_transferred);

    http::async_read(stream_, buffer_, res_,
        std::bind(&RestClient::on_read, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
}

void RestClient::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    if (ec) return fail(ec, "read");
    boost::ignore_unused(bytes_transferred);

    // Преобразуем буфер в std::string
    std::string response = boost::beast::buffers_to_string(res_.body().data());
    if (response_handler_) {
        response_handler_(response, ec);
    }

    // Закрытие сокета
    beast::error_code shutdown_ec;
    stream_.socket().shutdown(tcp::socket::shutdown_both, shutdown_ec);
    if (shutdown_ec && shutdown_ec != beast::errc::not_connected) {
        fail(shutdown_ec, "shutdown");
    }
}


void RestClient::fail(beast::error_code ec, const char* what) {
    std::cerr << what << ": " << ec.message() << "\n";
    if (response_handler_) {
        response_handler_("", ec);
    }
}