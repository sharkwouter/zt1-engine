cmake_minimum_required(VERSION 3.10)

project(zoo)

add_executable(${PROJECT_NAME} main.c)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(ZIP REQUIRED libzip)
pkg_search_module(STB REQUIRED stb)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${ZIP_LIBRARIES}
  ${SDL2_LIBRARIES}
  m
)
