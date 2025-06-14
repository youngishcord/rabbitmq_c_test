cmake_minimum_required(VERSION 3.10)

project(rabbitmq_c_test C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RABBITMQ REQUIRED librabbitmq)

add_executable(sender src/sender.c)
target_include_directories(sender PRIVATE ${RABBITMQ_INCLUDE_DIRS})
target_link_libraries(sender ${RABBITMQ_LIBRARIES})

add_executable(receiver src/receiver.c)
target_include_directories(receiver PRIVATE ${RABBITMQ_INCLUDE_DIRS})
target_link_libraries(receiver ${RABBITMQ_LIBRARIES})