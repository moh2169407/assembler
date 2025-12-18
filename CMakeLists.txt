cmake_minimum_required(VERSION 3.27.0)
project(masm)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(masm src/main.c src/fileio.c src/lexer.c src/log.c src/parser.c)
target_compile_options(masm PRIVATE -g -D LOG_USE_COLOR) #-g -Wall -Wextra -Werror)
target_include_directories(masm PUBLIC include)
target_include_directories(masm PUBLIC svec)
