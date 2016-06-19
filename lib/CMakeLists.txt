include(FindBSDQueue)

add_library(corewar STATIC
  corewar.c
  op.c
  opi.c
  ops.c
  load.c
  hooks.c
  gather.c
)
target_include_directories(corewar PUBLIC inc)
target_link_libraries(corewar ft bsd_queues)
