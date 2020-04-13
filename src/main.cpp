#include <iostream>
#include "listener.h"
#include "application.h"

int main(int argc , char *argv[]) {
    if (argc != 5) {
        std::cout << "Usage: " << std::string(argv[0]) << " ip port word tries" << std::endl;
        return -1;
    }

    auto app = application::get_current_application();
    app->word_ = std::string(argv[3]);
    app->tries_ = std::atoi(argv[4]);

    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::endpoint endpoint_(boost::asio::ip::address::from_string(argv[1]), std::atoi(argv[2]));

    listener listener_(ioc_, std::move(endpoint_));

    listener_.run();
    ioc_.run();

    return 0;
}
