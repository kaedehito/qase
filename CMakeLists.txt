cmake_minimum_required(VERSION 3.5)
project(qase C)


find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_mixer REQUIRED SDL2_mixer)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_image REQUIRED SDL2_image)
pkg_check_modules(ID3TAG REQUIRED libid3tag)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Werror -Wextra -g -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")


add_executable(qase src/main.c)


target_link_libraries(qase 
  ${SDL2_LIBRARIES}
  ${SDL2_mixer_LIBRARIES}
  ${SDL2_image_LIBRARIES}
  ${ID3TAG_LIBRARIES}
)


target_include_directories(qase PRIVATE
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_mixer_INCLUDE_DIRS}
  ${SDL2_image_INCLUDE_DIRS}
  ${ID3TAG_INCLUDE_DIRS}
)

