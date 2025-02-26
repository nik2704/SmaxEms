// RestClient.cpp
#include "RestClient.h"
#include <iostream>

RestClient::RestClient(net::io_context& ioc, ssl::context& ctx, const std::string& host, const std::string& port)
    : resolver_(ioc), stream_(ioc, ctx), host_(host), port_(port) {}

void RestClient::run(const std::string& target, http::verb method, 
                     const std::string& body, ResponseHandler handler,
                     const std::map<std::string, std::string>& headers) {
    req_.method(method);
    req_.target(target);
    req_.version(11);
    req_.set(http::field::host, host_);
    req_.set(http::field::user_agent,  "application/json");
    
    // Устанавливаем заголовки, если они переданы
    for (const auto& [key, value] : headers) {
        req_.set(key, value);
    }

    if (!body.empty()) {
        req_.set(http::field::content_type, "application/json");
        req_.body() = body;
        req_.prepare_payload();
    }

    response_handler_ = std::move(handler);
    resolver_.async_resolve(host_, port_, beast::bind_front_handler(&RestClient::on_resolve, shared_from_this()));
}


void RestClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec) return fail(ec, "resolve");

    beast::get_lowest_layer(stream_).async_connect(results,
        std::bind(&RestClient::on_connect, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
}
// void RestClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
//     std::cerr << "on_resolve called" << std::endl;
//     if (ec) {
//         std::cerr << "Resolve error: " << ec.message() << std::endl;
//         return fail(ec, "resolve");
//     }
//     if (results.empty()) {
//         std::cerr << "No endpoints found!" << std::endl;
//         return;
//     }

//     std::cerr << "Trying to connect to: " << results->endpoint() << std::endl;

//     beast::get_lowest_layer(stream_).async_connect(
//         results,
//         std::bind(&RestClient::on_connect, shared_from_this(), std::placeholders::_1, std::placeholders::_2)
//     );
// }


void RestClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
    if (ec) return fail(ec, "connect");

    stream_.async_handshake(ssl::stream_base::client,
        std::bind(&RestClient::on_handshake, shared_from_this(), std::placeholders::_1));
}

// void RestClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
//     std::cerr << "on_connect called" << std::endl;
//     if (ec) {
//         std::cerr << "Connect error: " << ec.message() << std::endl;
//         return fail(ec, "connect");
//     }
//     stream_.async_handshake(
//         ssl::stream_base::client,
//         std::bind(&RestClient::on_handshake, shared_from_this(), std::placeholders::_1)
//     );
// }


void RestClient::on_handshake(beast::error_code ec) {
    if (ec) return fail(ec, "handshake");

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

    if (response_handler_) {
        response_handler_(boost::beast::buffers_to_string(res_.body().data()), ec);
        response_handler_ = nullptr;  // Обнуляем handler, чтобы избежать повторного вызова
    }

    stream_.async_shutdown([self = shared_from_this()](beast::error_code shutdown_ec) {
        if (shutdown_ec && shutdown_ec != beast::errc::not_connected && shutdown_ec != boost::asio::ssl::error::stream_truncated) {
            self->fail(shutdown_ec, "shutdown");
        }
    });
}

// void RestClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
//     std::cerr << "on_resolve called" << std::endl;
//     if (ec) return fail(ec, "resolve");
//     beast::get_lowest_layer(stream_).async_connect(results,
//         std::bind(&RestClient::on_connect, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
// }

// void RestClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
//     std::cerr << "on_connect called" << std::endl;
//     if (ec) return fail(ec, "connect");
//     stream_.async_handshake(ssl::stream_base::client,
//         std::bind(&RestClient::on_handshake, shared_from_this(), std::placeholders::_1));
// }

// void RestClient::on_handshake(beast::error_code ec) {
//     std::cerr << "on_handshake called" << std::endl;
//     if (ec) return fail(ec, "handshake");
//     http::async_write(stream_, req_,
//         std::bind(&RestClient::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
// }

// void RestClient::on_write(beast::error_code ec, std::size_t bytes_transferred) {
//     std::cerr << "on_write called" << std::endl;
//     if (ec) return fail(ec, "write");
//     boost::ignore_unused(bytes_transferred);
//     http::async_read(stream_, buffer_, res_,
//         std::bind(&RestClient::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
// }

// void RestClient::on_read(beast::error_code ec, std::size_t bytes_transferred) {
//     std::cerr << "on_read called" << std::endl;
//     if (ec) return fail(ec, "read");
//     boost::ignore_unused(bytes_transferred);
//     response_handler_(boost::beast::buffers_to_string(res_.body().data()), ec);
//     stream_.async_shutdown([self = shared_from_this()](beast::error_code shutdown_ec) {
//         if (shutdown_ec && shutdown_ec != beast::errc::not_connected && shutdown_ec != boost::asio::ssl::error::stream_truncated) {
//             self->fail(shutdown_ec, "shutdown");
//         }
//     });
// }

// void RestClient::on_read(beast::error_code ec, std::size_t bytes_transferred) {
//     if (ec) return fail(ec, "read");
//     boost::ignore_unused(bytes_transferred);

//     std::cerr << "Response received: " << boost::beast::buffers_to_string(res_.body().data()) << std::endl;

//     if (response_handler_) {
//         response_handler_(boost::beast::buffers_to_string(res_.body().data()), ec);
//     }

//     stream_.async_shutdown([self = shared_from_this()](beast::error_code shutdown_ec) {
//         if (shutdown_ec && shutdown_ec != beast::errc::not_connected && shutdown_ec != boost::asio::ssl::error::stream_truncated) {
//             self->fail(shutdown_ec, "shutdown");
//         }
//     });
// }


void RestClient::fail(beast::error_code ec, const char* what) {
    std::cerr << what << ": " << ec.message() << "\n";
    if (response_handler_) {
        response_handler_("", ec);
    }
}
