cmake_minimum_required(VERSION 2.8)

project(sk-compiler)
add_executable(ltg-opt 
  ltg-opt.cpp expand-integer.cpp expand-sk.cpp command.cpp compile.cpp
  program.cpp
)
