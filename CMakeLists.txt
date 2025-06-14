cmake_minimum_required(VERSION 3.10)

project(rabbitmq_c_test C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RABBITMQ REQUIRED librabbitmq)

add_executable(sender src/sender.c)

target_include_directories(sender PRIVATE ${RABBITMQ_INCLUDE_DIRS})
target_link_libraries(sender ${RABBITMQ_LIBRARIES})