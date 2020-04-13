cmake_minimum_required(VERSION 3.10)
project(game)

include(GNUInstallDirs)
find_package(Boost 1.67 COMPONENTS system REQUIRED)
find_package(Threads)

set(CMAKE_CXX_STANDARD 17)

add_executable(game src/main.cpp src/connection.h src/listener.h src/application.h src/application.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})