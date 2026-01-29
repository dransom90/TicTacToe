cmake_minimum_required(VERSION 3.26)
project(tictactoe)

find_package(SDL3 REQUIRED)

add_executable(tictactoe tictactoe.c buttons.c)

target_include_directories(tictactoe PRIVATE .)


target_link_libraries(tictactoe SDL3::SDL3)