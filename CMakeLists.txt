cmake_minimum_required(VERSION 3.3)
project("corewar" C)

add_compile_options(-Wall -Wextra -Werror)
add_subdirectory(libft)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(lib)
add_subdirectory(cli)
