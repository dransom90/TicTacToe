cmake_minimum_required(VERSION 3.26)
project(tictactoe)

find_package(SDL3 REQUIRED)

add_executable(tictactoe src/tictactoe.c src/buttons.c)

target_include_directories(tictactoe PRIVATE 
                            ${CMAKE_CURRENT_SOURCE_DIR}/include
                            ${CMAKE_CURRENT_SOURCE_DIR/src})


target_link_libraries(tictactoe SDL3::SDL3)