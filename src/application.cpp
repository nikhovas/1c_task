#include "application.h"

std::shared_ptr<application> current_application(nullptr);

std::shared_ptr<application> application::get_current_application() {
    if (current_application == nullptr) {
        current_application = std::make_shared<application>();
    }
    return current_application;
}