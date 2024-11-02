cmake_minimum_required(VERSION 3.5)

project(qase C)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_library(ID3TAG_LIB id3tag REQUIRED)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -g -fsanitize=address")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

add_executable(qase src/main.c)

target_link_libraries(qase SDL2_image::SDL2_image SDL2::SDL2 id3tag SDL2_mixer::SDL2_mixer)

