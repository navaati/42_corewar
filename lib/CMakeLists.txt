include(FindBSDQueue)

add_library(corewar STATIC
  corewar.c
  ops.c
  )
target_include_directories(corewar PUBLIC inc)
target_link_libraries(corewar ft bsd_queues)
