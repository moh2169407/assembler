cmake_minimum_required(VERSION 3.27.0)
project(masm)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(masm src/main.c src/diagnostics.c src/log.c)
target_compile_options(masm PRIVATE -g -DLOG_USE_COLOR -DDEBUG_LOGGING -Wall -Wextra -Werror)
target_include_directories(masm PUBLIC include)
target_include_directories(masm PUBLIC svec)

