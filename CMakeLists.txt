cmake_minimum_required(VERSION 3.10)

project(zt1-engine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC})

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_search_module(ZIP REQUIRED libzip)

set(PE_BITMAP_LOADER OFF CACHE INTERNAL "Prevent pe-resource-loader from building the pe_bitmap_loader binary" FORCE)
set(PE_STRING_LOADER OFF CACHE INTERNAL "Prevent pe-resource-loader from building the pe_string_loader binary" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/pe-resource-loader)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${ZIP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/pe-resource-loader/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${ZIP_LIBRARIES}
    pe_resource_loader
)
