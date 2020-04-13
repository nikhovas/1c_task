#pragma once

#include "connection.h"

class listener {
public:
    listener(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint&& endpoint) : acceptor_(ioc, endpoint) { }

    void run() {
        start_accept();
    }

private:
    void start_accept() {
        acceptor_.async_accept(std::bind(&listener::on_accept, this, std::placeholders::_1,
                std::placeholders::_2));
    }

    void on_accept(std::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            std::make_shared<session>(std::move(socket))->run();
        }

        start_accept();
    }

    boost::asio::ip::tcp::acceptor acceptor_;
};