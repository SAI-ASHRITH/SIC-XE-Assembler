#include "pass_two.h"

#include "io.h"

#include "load_instruction.h"
#include "pass_one.h"

/**
 * CONSTANTS and DEFINES
 */

/**
 * STATIC FUNCTIONS
 */
static void pass_two__create_char_array(GIOChannel *channel);
static void pass_two__parse_instruction_line(gchar *instruction_line,
                                             gchar *return_label,
                                             gchar *return_opcode,
                                             gchar *return_data);

static gchar *pass_two__generic_case(gchar *opcode, gchar *data);
static gboolean pass_two__special_case(const gchar *label, const gchar *opcode,
                                       const gchar *data, gchar **return_data);
static gchar *pass_two__special_case_for_byte(const gchar *opcode,
                                              const gchar *data);

// Helper
static gchar *pass_two__create_allocated_string_from_static_string(
    const gchar *static_string);

/**
 * STATIC VARIABLES
 */
static GIOChannel *read_channel;
static gchar **split_string;

/**
 * FUNCTIONS
 */

gboolean pass_two__run(const gchar *filename) {
  // Create the intermediate filename here
  gchar intermediate_filename[50] = {0};
  strcpy(intermediate_filename, filename);
  strcat(intermediate_filename, ".intermediate");

  // Open the file first
  gboolean opened;
  read_channel = io__open_file(intermediate_filename, "r", &opened);
  if (!opened) {
    return FALSE;
  }

  // Create an array
  pass_two__create_char_array(read_channel);

  // Close the file
  io__close_file(read_channel, TRUE);
}

void pass_two__clean(void) { g_strfreev(split_string); }

guint pass_two__get_instruction_array_size(void) {
  return g_strv_length(split_string);
}

gchar *pass_two__parse_instruction_at_index(guint32 index, guint32 *locctr) {
  guint size = pass_two__get_instruction_array_size();
  if (index >= size) {
    g_printerr("[%s] -> Index out of bounds (index >= instruction_arr_size\r\n",
               __FUNCTION__);
    return NULL;
  }

  if (locctr != NULL) {
    pass_one__get_index_locctr(index, locctr);
  }

  // Get line at index
  gchar *instruction = split_string[index];
  // ? debugging
  // g_print("Instruction: %s\r\n", instruction);

  // Instruction can be empty as well
  if (instruction == NULL || strcmp(instruction, "") == 0) {
    // g_printerr("Instruction is NULL\r\n");
    return NULL;
  }

  // Process the data here
  gchar label[20];
  gchar opcode[20];
  gchar data[20];
  pass_two__parse_instruction_line(instruction, label, opcode, data);

  // DONE, Get START, END
  // DONE, WORD, BYTE
  // DONE, RESW, RESB
  gchar *return_data = NULL;
  if (pass_two__special_case(label, opcode, data, &return_data)) {
    return return_data;
  }

  return pass_two__generic_case(opcode, data);
}

/**
 * STATIC DEFINITIONS
 */
static void pass_two__create_char_array(GIOChannel *channel) {
  gchar *str_arr = NULL;
  gsize length = 0;
  GError *error = NULL;
  g_io_channel_read_to_end(channel, &str_arr, &length, &error);

  if (error != NULL) {
    g_printerr("[%s] -> %s\r\n", __FUNCTION__, error->message);
  }

  // ? debugging
  // g_print("Error: %p\r\n", error);
  // g_print("Length: %d\r\n", length);
  // for (int i = 0; i < length; i++) {
  //   g_print("%c : %x\r\n", str_arr[i], str_arr[i]);
  // }

  split_string = g_strsplit(str_arr, "\r\n", -1);
  g_free(str_arr);
}

static void pass_two__parse_instruction_line(gchar *instruction_line,
                                             gchar *return_label,
                                             gchar *return_opcode,
                                             gchar *return_data) {
  gchar **split_string = g_strsplit(instruction_line, ",", 3);

  // ? debugging
  // for (int i = 0; i < 3; i++) {
  //   g_print("%s\r\n", split_string[i]);
  // }

  strcpy(return_label, split_string[0]);
  strcpy(return_opcode, split_string[1]);
  strcpy(return_data, split_string[2]);

  g_strfreev(split_string);
}

static gchar *pass_two__generic_case(gchar *opcode, gchar *data) {
  // Create the instructions
  instruction_t i = {};
  guint32 location = 0;

  load_instruction__get_opcode_from_mnemonic(opcode, &i);
  pass_one__get_symtab_locctr(data, &location);

  // Create the static buffer here
  gchar tbuffer[10] = {0};
  g_snprintf(tbuffer, sizeof(tbuffer) / sizeof(gchar), "%02x%04x", i.opcode,
             location);

  // ? debugging
  // g_print("%s -> %d\r\n", tbuffer, strlen(tbuffer));
  return pass_two__create_allocated_string_from_static_string(tbuffer);
}

static gboolean pass_two__special_case(const gchar *label, const gchar *opcode,
                                       const gchar *data, gchar **return_data) {
  gboolean rval = FALSE;
  *return_data = NULL;

  char tbuffer[30] = {0};

  if (strcmp(opcode, "START") == 0) {
    // DONE, Return H record
    guint32 start_address = g_ascii_strtoull(data, NULL, 16);
    guint32 program_size = pass_one__get_program_size();
    g_snprintf(tbuffer, sizeof(tbuffer) / sizeof(gchar), "H%s %06x%06x", label,
               start_address, program_size);

    // ? debugging
    // g_print("START: %s\r\n", tbuffer);

    *return_data =
        pass_two__create_allocated_string_from_static_string(tbuffer);
    rval = TRUE;
  } else if (strcmp(opcode, "END") == 0) {
    // TODO, Return E record
    guint32 locctr = 0;
    pass_one__get_symtab_locctr((gchar *)data, &locctr);
    g_snprintf(tbuffer, sizeof(tbuffer) / sizeof(gchar), "E%06x", locctr);

    // ? debugging
    // g_print("E record: %s\r\n", tbuffer);

    *return_data =
        pass_two__create_allocated_string_from_static_string(tbuffer);
    rval = TRUE;
  } else if (strcmp(opcode, "WORD") == 0) {
    // DONE, Convert to object code
    guint32 ui32data = g_ascii_strtoull(data, NULL, 10);
    g_snprintf(tbuffer, sizeof(tbuffer) / sizeof(gchar), "%06x", ui32data);

    // ? debugging
    // g_print("BUFFERS: %s\r\n", tbuffer);

    *return_data =
        pass_two__create_allocated_string_from_static_string(tbuffer);
    rval = TRUE;
  } else if (strcmp(opcode, "BYTE") == 0) {
    // DONE, Convert to necessary object code
    *return_data = pass_two__special_case_for_byte(opcode, data);
    rval = TRUE;
  } else if (strcmp(opcode, "RESW") == 0) {
    // Does nothing
    rval = TRUE;
  } else if (strcmp(opcode, "RESB") == 0) {
    // Does nothing
    rval = TRUE;
  }

  return rval;
}

static gchar *pass_two__special_case_for_byte(const gchar *opcode,
                                              const gchar *data) {
  gboolean found = FALSE;
  guint32 counter = 0;
  gchar buffer[20] = {0};
  for (int i = 1; i < strlen(data); i++) {

    if (found == TRUE) {
      if (data[i] == '\'') {
        break;
      }
      // Start writing here
      buffer[counter] = data[i];
      counter++;
    }

    if (data[i] == '\'' && found == FALSE) {
      counter = 0;
      found = TRUE;
    }
  }

  // ? debugging
  // g_print("Data: %s %d\r\n", buffer, counter);

  gchar rvalBuffer[20] = {0};
  if (data[0] == 'C') {
    for (int i = 0; i < counter; i++) {
      gchar conversionBuffer[3] = {0};
      g_snprintf(conversionBuffer, sizeof(conversionBuffer) / sizeof(gchar),
                 "%02x", buffer[i]);
      strcat(rvalBuffer, conversionBuffer);
      // g_print("%c %x %s\r\n", buffer[i], buffer[i], rvalBuffer);
    }
    // END
  } else if (data[0] == 'X') {
    guint32 value = g_ascii_strtoull(buffer, NULL, 16);
    g_snprintf(rvalBuffer, sizeof(rvalBuffer) / sizeof(gchar), "%02x", value);
  }

  // g_print("%s\r\n", rvalBuffer);
  return pass_two__create_allocated_string_from_static_string(rvalBuffer);
}

// Helper
/**
 * @brief Takes a static string and allocates memory on the heap
 * ! IMP, Do not forget to free the memory
 *
 * @param static_string
 * @return gchar*
 */
static gchar *pass_two__create_allocated_string_from_static_string(
    const gchar *static_string) {
  gchar *rstr = g_new(gchar, strlen(static_string));
  strcpy(rstr, static_string);
  return rstr;
}
