cmake_minimum_required(VERSION 3.20)
project(king_game)
add_executable(king_game King/main.cpp)
target_compile_features(king_game PRIVATE cxx_std_17)
