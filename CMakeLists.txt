cmake_minimum_required(VERSION 3.3)
project("corewar" C)

add_compile_options(-Wall -Wextra -Werror)

add_subdirectory(libft)

include(FindBSDQueue.cmake)

add_executable(corewar corewar.c)
target_link_libraries(corewar ft bsd_queues)
