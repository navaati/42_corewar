include(CheckSymbolExists)

set(SYSTEM_QUEUE_H sys/queue.h)
set(LIBBSD_QUEUE_H bsd/sys/queue.h)

add_library(bsd_queues INTERFACE)

message(STATUS "In <${SYSTEM_QUEUE_H}>:")
check_symbol_exists("LIST_FOREACH_SAFE" "${SYSTEM_QUEUE_H}" LIST_FOREACH_SAFE_EXISTS)
if(LIST_FOREACH_SAFE_EXISTS)
  # Simply use #include <sys/queue.h>
else()
  message(STATUS "In <${LIBBSD_QUEUE_H}>:")
  check_symbol_exists("LIST_FOREACH_SAFE" "${LIBBSD_QUEUE_H}" LIST_FOREACH_SAFE_EXISTS_LIBBSD)
  if(LIST_FOREACH_SAFE_EXISTS_LIBBSD)
    # Use #include <bsd/sys/queue.h> (libbsd)
    target_compile_definitions(bsd_queues INTERFACE USE_LIBBSD_QUEUE_H)
  else()
    message(SEND_ERROR "Cannot find a sys/queue.h with LIST_FOREACH_SAFE macro")
  endif()
endif()
