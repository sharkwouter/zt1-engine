cmake_minimum_required(VERSION 3.10)

project(zt1-engine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC})

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)

set(PE_BITMAP_LOADER OFF CACHE INTERNAL "Prevent pe-resource-loader from building the pe_bitmap_loader binary" FORCE)
set(PE_STRING_LOADER OFF CACHE INTERNAL "Prevent pe-resource-loader from building the pe_string_loader binary" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/pe-resource-loader)

set(SDL_SHARED OFF CACHE INTERNAL "" FORCE)
set(SDL_STATIC ON CACHE INTERNAL "" FORCE)
set(SDL2_DISABLE_INSTALL ON CACHE INTERNAL "" FORCE)
set(SDL_TEST OFF CACHE INTERNAL "" FORCE)
add_subdirectory(vendor/SDL)

set(SDL2IMAGE_SAMPLES OFF)
set(SDL2IMAGE_AVIF OFF)
set(SDL2IMAGE_BMP ON)
set(SDL2IMAGE_GIF OFF)
set(SDL2IMAGE_JPG OFF)
set(SDL2IMAGE_JXL OFF)
set(SDL2IMAGE_LBM OFF)
set(SDL2IMAGE_PCX OFF)
set(SDL2IMAGE_PNG OFF)
set(SDL2IMAGE_PNM OFF)
set(SDL2IMAGE_QOI OFF)
set(SDL2IMAGE_SVG OFF)
set(SDL2IMAGE_TGA ON)
set(SDL2IMAGE_TIF OFF)
set(SDL2IMAGE_WEBP OFF)
set(SDL2IMAGE_XCF OFF)
set(SDL2IMAGE_XPM OFF)
set(SDL2IMAGE_XV OFF)
set(SDL2IMAGE_DEPS_SHARED OFF)
set(SDL2IMAGE_VENDORED OFF)
set(SDL2IMAGE_BACKEND_IMAGEIO OFF)
set(SDL2IMAGE_BACKEND_STB OFF)
add_subdirectory(vendor/SDL_image)

set(SDL2MIXER_DEPS_SHARED OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_VENDORED ON CACHE INTERNAL "" FORCE)
set(SDL2MIXER_SAMPLES OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_CMD OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_FLAC OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_OPUS OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_VORBIS OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_MOD OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_WAVPACK OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_MIDI OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_MP3 OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_MP3_MINIMP3 OFF CACHE INTERNAL "" FORCE)
set(SDL2MIXER_WAVE ON CACHE INTERNAL "" FORCE)
add_subdirectory(vendor/SDL_mixer)

set(SDL2TTF_BUILD_SHARED_LIBS OFF)
set(SDL2TTF_VENDORED ON)
set(SDL2TTF_SAMPLES OFF)
set(SDL2TTF_FREETYPE ON)
set(SDL2TTF_FREETYPE_VENDORED ON)
set(SDL2TTF_HARFBUZZ OFF)
set(SDL2TTF_HARFBUZZ_VENDORED OFF)
set(SDL2TTF_SAMPLES OFF)
set(SDL2TTF_SAMPLES OFF)
set(SDL2TTF_SAMPLES OFF)
set(SDL2TTF_SAMPLES OFF)
set(SDL2TTF_SAMPLES OFF)
add_subdirectory(vendor/SDL_ttf)

set(ENABLE_COMMONCRYPTO OFF CACHE INTERNAL "" FORCE)
set(ENABLE_GNUTLS OFF CACHE INTERNAL "" FORCE)
set(ENABLE_MBEDTLS OFF CACHE INTERNAL "" FORCE)
set(ENABLE_OPENSSL OFF CACHE INTERNAL "" FORCE)
set(ENABLE_WINDOWS_CRYPTO OFF CACHE INTERNAL "" FORCE)
set(ENABLE_FDOPEN OFF CACHE INTERNAL "" FORCE)
set(BUILD_TOOLS OFF CACHE INTERNAL "" FORCE)
set(BUILD_REGRESS OFF CACHE INTERNAL "" FORCE)
set(BUILD_OSSFUZZ OFF CACHE INTERNAL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(BUILD_DOC OFF CACHE INTERNAL "" FORCE)
set(LIBZIP_DO_INSTALL OFF CACHE INTERNAL "" FORCE)
set(SHARED_LIB_VERSIONNING OFF CACHE INTERNAL "" FORCE)
set(ENABLE_BZIP2 OFF CACHE INTERNAL "" FORCE)
set(ENABLE_LZMA OFF CACHE INTERNAL "" FORCE)
set(ENABLE_ZSTD OFF CACHE INTERNAL "" FORCE)
add_subdirectory(vendor/libzip)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/pe-resource-loader/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL_image/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL_ttf/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL_mixer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/libzip/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pe_resource_loader
    SDL2::SDL2-static
    SDL2_image
    SDL2_ttf
    SDL2_mixer
    libzip::zip
)

if(WIN32)
    set(CMAKE_CXX_FLAGS "-static ${CMAKE_CXX_FLAGS}")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2main
    )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/font ${CMAKE_CURRENT_BINARY_DIR}/font)
