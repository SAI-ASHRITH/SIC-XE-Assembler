#include "object_file_builder.h"

#include <string.h>

#include "io.h"
#include "pass_two.h"

typedef struct {
  gchar **instruction_string_array;
  guint32 *locctr_array;
  size_t total_array_length;
} instruction_s;

static instruction_s instructions;

void object_file_builder__initialize(guint arr_size) {
  instructions.total_array_length = arr_size;
  instructions.instruction_string_array =
      g_new(gchar *, instructions.total_array_length);
  instructions.locctr_array = g_new(guint32, instructions.total_array_length);
}

void object_file_builder__free(void) {
  for (size_t i = 0; i < instructions.total_array_length; i++) {
    g_free(instructions.instruction_string_array[i]);
  }
  g_free(instructions.instruction_string_array);
  g_free(instructions.locctr_array);
}

void object_file_builder__populate_string_array(size_t i) {
  instructions.instruction_string_array[i] = g_new(gchar, 50);
  instructions.locctr_array[i] = 0;

  gchar *instruction =
      pass_two__parse_instruction_at_index(i, &instructions.locctr_array[i]);

  strcpy(instructions.instruction_string_array[i],
         instruction != NULL ? instruction : "");
  g_print("%d -> %s : %x\r\n", i, instructions.instruction_string_array[i],
          instructions.locctr_array[i]);

  g_free(instruction);
}

void object_file_builder__create_object_file(const gchar *filename) {
  // Create the filename
  gchar object_filename[50] = {0};
  strcpy(object_filename, filename);
  strcat(object_filename, ".o");

  gboolean status;
  GIOChannel *clean_channel = io__open_file(object_filename, "w", &status);
  io__close_file(clean_channel, FALSE);

  GIOChannel *object_file = io__open_file(object_filename, "a", &status);

  gchar line[100];

  // Append HRECORD
  sprintf(line, "%s\n", instructions.instruction_string_array[0]);
  io__write_file(object_file, line);

  // TRECORD append entries
  size_t instructions_current_index = 1;
  gboolean end_record_not_found = TRUE;

  while (end_record_not_found) {
    size_t trecord_starting_address =
        instructions.locctr_array[instructions_current_index];

    // 60 total entries including null terminator
    gchar trecord_object_code_string[61] = {'\0'};

    while (
        sizeof(trecord_object_code_string) >
        (strlen(trecord_object_code_string) +
         strlen(instructions
                    .instruction_string_array[instructions_current_index]))) {

      size_t current_locctr =
          instructions.locctr_array[instructions_current_index];
      size_t next_locctr =
          instructions.locctr_array[instructions_current_index + 1];
      gchar *current_string =
          instructions.instruction_string_array[instructions_current_index];

      if (current_string[0] == 'E') {
        // Found ERECORD
        end_record_not_found = FALSE;
        break;
      } else {
        // append opcode
        strcat(trecord_object_code_string, current_string);
        instructions_current_index++;

        // Continuous opcode locctr's are 3 bytes or less
        if (next_locctr > (current_locctr + 3)) {
          // Jump found
          break;
        }
      }
    }

    const size_t trecord_total_bytes = strlen(trecord_object_code_string) / 2;
    sprintf(line, "T%06x%02x%s\n", trecord_starting_address,
            trecord_total_bytes, trecord_object_code_string);
    io__write_file(object_file, line);
  }

  // Append ERECORD
  sprintf(line, "%s\n",
          instructions.instruction_string_array[instructions_current_index]);
  io__write_file(object_file, line);

  io__close_file(object_file, TRUE);
}
