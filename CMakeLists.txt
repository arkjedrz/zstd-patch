cmake_minimum_required(VERSION 3.10)

project(zstd-patch C)

# Global settings.
add_compile_options(-Wall -Wextra)

add_subdirectory("src/")