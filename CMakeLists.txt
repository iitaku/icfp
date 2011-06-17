project(bot)
cmake_minimum_required(VERSION 2.8)

add_executable(bot bot.cpp)

enable_testing()

add_test(test ./ltg.macosx-10.6-intel match ./bot ./run.sh 2> log &)
