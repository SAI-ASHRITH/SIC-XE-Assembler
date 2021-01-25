message("Loading Utility Source Dir")

message("list_dir: ${CMAKE_CURRENT_LIST_DIR}")

target_sources(${USER_PROJECT} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/io.c)
target_include_directories(${USER_PROJECT} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
