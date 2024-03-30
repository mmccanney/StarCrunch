cmake_minimum_required(VERSION 3.16)
project(StarCrunch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(StarCrunch src/main.cpp)
target_link_libraries(StarCrunch PRIVATE sfml-graphics sfml-audio)
target_compile_features(StarCrunch PRIVATE cxx_std_17)

file(COPY assets/img/rock.png
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
file(COPY assets/font/LiberationMono-Bold.ttf
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
file(COPY assets/font/UbuntuMono-B.ttf
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
file(COPY assets/sound/pew.ogg
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

install(TARGETS StarCrunch)
