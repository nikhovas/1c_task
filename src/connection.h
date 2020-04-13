#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>
#include "application.h"


class session : public std::enable_shared_from_this<session> {
public:
    session(boost::asio::ip::tcp::socket&& socket) : socket_(std::move(socket)),
    word_(application::get_current_application()->word_),
    more_tries_(application::get_current_application()->tries_),
    word_mask_(word_.size(), false),
    in_game_(false) { }

    void run() {
        word_ = application::get_current_application()->word_;
        more_tries_ = application::get_current_application()->tries_;
        word_mask_ = std::vector<bool>(word_.size(), false);
        in_game_ = true;
        send_new_message();
    }

    void start_loop() {
        socket_.async_read_some(boost::asio::buffer(symbol, 1024),
                                std::bind(&session::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void on_read(std::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
            if (symbol[0] == '/') {
                read_cmd(bytes_transferred);
                return;
            }

            if (!in_game_) {

                start_loop();
                return;
            }

            --more_tries_;

            for (std::size_t i = 0; i < word_.size(); ++i) {
                if (word_[i] == symbol[0]) {
                    word_mask_[i] = true;
                }
            }

            bool is_win_ = true;
            for (auto i : word_mask_) {
                if (!i) {
                    is_win_ = false;
                    break;
                }
            }

            if (is_win_) {
                in_game_ = false;
                send_raw_message("You won!!!! Congratulations!!!   /new for new game, /exit for exit\n");
            } else if (more_tries_ == 0) {
                in_game_ = false;
                send_raw_message("You lost :(   /new for new game, /exit for exit\n");
            } else {
                send_new_message();
            }

        }
    }

    void read_cmd(std::size_t bytes_transferred) {
        if (memcmp(symbol, "/new", 4) == 0) {
            str_ = "Game reloaded\n";
            socket_.async_write_some(boost::asio::buffer(str_.c_str(), str_.size()),
                                     std::bind(&session::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
            run();
        } else if (memcmp(symbol, "/status", 7) == 0) {
            send_new_message();
        } else if (memcmp(symbol, "/exit", 5) == 0) {

            do_close();
        } else {
            send_raw_message("Unknown command\n");
        }
    }

    void send_new_message() {
        str_ = "";
        for (std::size_t i = 0; i < word_.size(); ++i) {
            if (word_mask_[i]) {
                str_.push_back(word_[i]);
            } else {
                str_.push_back('*');
            }
        }

        str_ += ", tries left: " + std::to_string(more_tries_) + "\n";

        socket_.async_write_some(boost::asio::buffer(str_.c_str(), str_.size()),
                                 std::bind(&session::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void send_raw_message(const std::string& msg) {
        str_ = msg;
        socket_.async_write_some(boost::asio::buffer(str_.c_str(), str_.size()),
                                 std::bind(&session::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void on_write(std::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
            start_loop();
        }
    }

    void do_close() {
        str_ = "Goodbuy!\n";
        socket_.async_write_some(boost::asio::buffer(str_.c_str(), str_.size()),
                                 std::bind(&session::on_close, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void on_close(std::error_code ec, std::size_t bytes_transferred) {
        socket_.close();
    }

private:
    boost::asio::ip::tcp::socket socket_;
    char symbol[1024];
    std::string word_;
    std::vector<bool> word_mask_;
    uint64_t more_tries_;
    std::string str_;
    bool in_game_;
};