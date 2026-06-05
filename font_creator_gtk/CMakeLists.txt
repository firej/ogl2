cmake_minimum_required(VERSION 3.16)
project(font_creator_gtk CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(PANGOCAIRO REQUIRED IMPORTED_TARGET pangocairo)

add_executable(font_creator_gtk
    src/main.cpp
    src/fontfile.cpp
)

target_link_libraries(font_creator_gtk PRIVATE
    PkgConfig::GTK4
    PkgConfig::PANGOCAIRO
)
