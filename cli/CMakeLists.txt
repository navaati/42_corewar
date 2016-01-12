add_executable(cli main.c)
target_link_libraries(cli corewar)
set_target_properties(cli PROPERTIES OUTPUT_NAME "corewar")
