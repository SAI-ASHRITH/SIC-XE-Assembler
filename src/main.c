#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "io.h"
#include "load_instruction.h"
#include "thread.h"
#include "timer.h"

#include "object_file_builder.h"
#include "pass_one.h"
#include "pass_two.h"

#include "command_line.h"

/**
 * Static Variables
 */
static const gchar INSTRUCTION_FILE[] = "instruction.csv";

/**
 * STATIC FUNCTIONS
 */

/**
 * MAIN
 */
int main(int argc, char **argv) {
  // Parse the command line arguments here
  command_line__init(argc, argv);

  unsigned char *aes_key = command_line__get_aes_key();
  gchar *filename = command_line__get_filename();
  guint thread_num = command_line__get_threads();

  // ? debugging
  // g_print("AES KEY: \"%s\"\r\n", aes_key);

  // Load the instructions
  gboolean instruction_loaded = load_instruction__load(INSTRUCTION_FILE);
  if (!instruction_loaded) {
    g_error("Instruction File could not be loaded\r\n");
  }

  // Run the Pass One algorithm
  gboolean pass_one_success = pass_one__run(filename);
  if (!pass_one_success) {
    g_error("Incorrect Filename -> \"%s\"\r\n", filename);
  }

  // ? debugging program size
  guint32 program_size = pass_one__get_program_size();
  g_print("program_size: %x\r\n", program_size);

  // Run the Pass Two algorithm
  gboolean pass_two_success = pass_two__run(filename);
  if (!pass_two_success) {
    g_error("Incorrect Filename -> \"%s\"\r\n", filename);
  }

  // ? debgging get the instructions
  guint arr_size = pass_two__get_instruction_array_size();
  g_print("Instruction Array Size: %d\r\n", arr_size);

  if (thread_num >= arr_size) {
    g_warning("Thread Size was %d when Total Instruction was %d\r\n",
              thread_num, arr_size);
    thread_num = arr_size / 2;
    g_warning("Resizing Thread Size to %d\r\n", thread_num);
  }

  // Object File init
  object_file_builder__initialize(arr_size);

  // Start the Timer
  timer__start();

  // Dynamically spin up threads
  thread__runner(arr_size, thread_num,
                 object_file_builder__populate_string_array);

  // ? Debugging, Get the performance measurement
  g_print("Program Elapsed Time: %f\r\n", timer__get_elapsed());

  // Cleanup
  timer__destroy();
  pass_two__clean();

  // TODO, Display onto the screen

  // Write to File
  object_file_builder__create_object_file(filename);
  object_file_builder__free();

  return 0;
}
