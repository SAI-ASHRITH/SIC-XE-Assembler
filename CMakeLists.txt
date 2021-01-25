cmake_minimum_required(VERSION 3.16.0)

# User Constant

set(USER_PROJECT "sicxe_assembler")

# END

# CMAKE Constant

# Top Level so that we can find the test.glade file easily
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

# END

project(${USER_PROJECT})
add_executable(${USER_PROJECT} src/main.c)

target_sources(
  ${USER_PROJECT}
  PRIVATE src/load_instruction.c
          src/pass_one.c
          src/timer.c
          src/thread.c
          src/pass_two.c
          src/object_file_builder.c
          src/command_line.c)
target_include_directories(${USER_PROJECT} PRIVATE src)

include(src/utility/utility.cmake)

# PkgConfig
find_package(PkgConfig REQUIRED)

# GTK pkgConfig
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
target_link_libraries(${USER_PROJECT} PUBLIC PkgConfig::GTK3)
message("gtk: ${GTK3_FOUND}")
