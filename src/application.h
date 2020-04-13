#pragma once

#include <string>
#include <memory>

struct application {
    std::string word_;
    uint64_t tries_;

    static std::shared_ptr<application> get_current_application();
};
