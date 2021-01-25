#include "pass_one.h"

#include <gmodule.h>

#include "io.h"
#include "load_instruction.h"

/**
 * STATIC FUNCTION DECLARATIONS
 */
// PASS 1 SPECIFIC
static void pass_one__increment_locctr(const gchar *opcode, const gchar *data);
static guint32 pass_one__increment_locctr_on_byte(const gchar *opcode,
                                                  const gchar *data);

static void pass_one__update_locctr(guint32 address);
static guint32 pass_one__get_locctr(void);

static void pass_one__update_starting_address(guint32 address);
static void pass_one__update_end_address(guint32 address);

// HashTable
static void pass_one__create_hash_table(void);
static gboolean pass_one__insert_to_hash_table(gchar *key, guint32 *value);

static void pass_one__create_index_table(void);
static gboolean pass_one__insert_to_index_table(guint32 key, guint32 value);

// FILEIO

static void pass_one__load_cb(gchar *str);
static void pass_one__extract_data_from_string(gchar *str, gchar *return_str);

static void pass_one__get_token_from_split_string(gchar **split_string,
                                                  gchar *label, gchar *opcode,
                                                  gchar *data);

static gboolean pass_one__find_start_symbol(gchar *opcode, gchar *data);

static gboolean pass_one__find_end_symbol(gchar *str);
static void pass_one__update_symtab(gchar *label);

static void pass_one__write_intermediate_instruction(gchar *label,
                                                     gchar *opcode,
                                                     gchar *data);

/**
 * STATIC VARIABLES
 */
static GHashTable *symtab;
static GHashTable *indextab;

static guint32 locctr;
static guint32 starting_address;
static guint32 end_address;
static guint32 pass_one_cb_counter;

// TODO, Remove this later
static guint32 program_size;

static GIOChannel *intermediate_channel;

/**
 * FUNCTIONS
 */
gboolean pass_one__run(const gchar *filename) {
  // Create the hash table
  pass_one__create_hash_table();
  pass_one__create_index_table();

  // Create the intermediate file
  gchar intermediate_file[50] = {0};
  strcpy(intermediate_file, filename);
  strcat(intermediate_file, ".intermediate");

  g_print("Intermediate File: %s\r\n", intermediate_file);

  // open the file
  gboolean opened;
  GIOChannel *channel = io__open_file(filename, "r", &opened);
  if (!opened) {
    return FALSE;
  }

  // Bandaid solution, Open the file in write mode to clean it
  // Close it immediately after
  // TODO, Delete the intermediate file here
  gboolean clean;
  GIOChannel *clean_channel = io__open_file(intermediate_file, "w", &clean);
  io__close_file(clean_channel, FALSE);

  // read instructions one by one and store them into a buffer
  gboolean created;
  intermediate_channel = io__open_file(intermediate_file, "a", &created);
  if (!created) {
    return FALSE;
  }
  io__read_file(channel, pass_one__load_cb);

  // Close the channels
  io__close_file(intermediate_channel, TRUE);
  io__close_file(channel, TRUE);

  return TRUE;
}

gboolean pass_one__get_symtab_locctr(gchar *label, guint32 *locctr) {

  gboolean rval = FALSE;
  guint32 *value = g_hash_table_lookup(symtab, label);

  // ? debugging
  // g_print("Value: %p\r\n", value);

  if (value != NULL) {
    rval = TRUE;
    if (locctr != NULL) {
      *locctr = *value; // copy
    }
  }

  return rval;
}

gboolean pass_one__get_index_locctr(guint32 index, guint32 *locctr) {
  gboolean rval = FALSE;

  // Offset value (index 0 is stored at indextab 1)
  index++; // increment by 1

  // offset
  guint32 *value = g_hash_table_lookup(indextab, &index);
  // g_print("Value: %p\r\n", value);

  if (value != NULL) {
    rval = TRUE;
    // g_print("Value: %x\r\n", *value);
    if (locctr != NULL) {
      *locctr = *value;
    }
  }

  return rval;
}

guint32 pass_one__get_program_size(void) {
  return end_address - starting_address;
}

/**
 * STATIC FUNCTION DEFINITIONS
 */

// PASS ONE SPECIFIC
// TODO, Update this based on the opcode instruction
static void pass_one__increment_locctr(const gchar *str, const gchar *data) {
  // Other cases here
  guint32 value;
  if (strcmp(str, "WORD") == 0) {
    locctr += 3;
  } else if (strcmp(str, "RESW") == 0) {

    value = (guint32)g_ascii_strtoull(data, NULL, 10);

    // ? debugging
    // g_print("op: [RESW] data, %s\r\n", data);
    // g_print("op: [RESW] value, %d\r\n", value);

    locctr += (value * 3);

  } else if (strcmp(str, "RESB") == 0) {

    value = (guint32)g_ascii_strtoull(data, NULL, 10);

    // ? debugging
    // g_print("op: [RESB] data, %s\r\n", data);
    // g_print("op: [RESB] value, %d\r\n", value);

    locctr += value;

  } else if (strcmp(str, "BYTE") == 0) {
    // DONE, Get Byte length
    guint32 increment = pass_one__increment_locctr_on_byte(str, data);

    // ? debugging
    // g_print("Increment: %d\r\n", increment);

    locctr += increment;
  } else {
    // default case
    locctr += 3;
  }
}

static guint32 pass_one__increment_locctr_on_byte(const gchar *opcode,
                                                  const gchar *data) {
  guint8 divider = 1;
  if (data[0] == 'C') {
    divider = 1;
  } else if (data[0] == 'X') {
    divider = 2;
  }

  gboolean started = FALSE;
  guint32 counter = 0;
  for (int i = 1; i < strlen(data); i++) {

    // ? debugging
    // g_print("d: %c\r\n", data[i]);

    if (data[i] == '\'' && started == TRUE) {
      // Get the length here
      break;
    }

    // Count the number of chars
    counter++;

    if (data[i] == '\'' && started == FALSE) {
      counter = 0;
      started = TRUE;
    }
  }

  return (guint32)(counter / divider);
}

static void pass_one__update_locctr(guint32 address) { locctr = address; }

static guint32 pass_one__get_locctr(void) { return locctr; }

static void pass_one__update_starting_address(guint32 address) {
  starting_address = address;
}

static void pass_one__update_end_address(guint32 address) {
  end_address = address;
}

// IO
static void pass_one__load_cb(gchar *str) {
  // Do not process the comment
  if (str[0] == '.') {
    return;
  }

  // Update the state variables
  pass_one_cb_counter++;

  // ? debugging
  // g_print("%s -> %s\r\n", __FUNCTION__, str);
  // for (int i = 0; i < strlen(str); i++) {
  //   g_print("%c : %x\r\n", str[i], str[i]);
  // }

  gchar buffer[100] = {};
  pass_one__extract_data_from_string(str, buffer);

  guint g_length = strlen(buffer);
  // ? debugging
  // g_print("g_length: %d\r\n", g_length);
  if (g_length <= 0) {
    return;
  }

  // Split the formatted string
  gchar **split = g_strsplit(buffer, " ", -1);

  guint32 counter = 0;

  // ? debugging
  // g_print("%s -> %s\r\n", __FUNCTION__, buffer);
  // while (split[counter] != NULL) {
  //   g_print("%d -> %s\r\n", counter, split[counter]);
  //   counter++;
  // }

  gchar label[20] = {0};
  gchar opcode[20] = {0};
  gchar data[20] = {0};
  pass_one__get_token_from_split_string(split, label, opcode, data);
  g_strfreev(split);

  // ? debugging
  // g_print("label: %s\r\n", label);
  // g_print("opcode: %s\r\n", opcode);
  // g_print("data: %s\r\n", data);
  // g_print("-----\r\n");

  // START COMPUTATION FROM HERE
  pass_one__write_intermediate_instruction(label, opcode, data);
  pass_one__update_symtab(label);
  // g_print("counter: %d %x\r\n", pass_one_cb_counter, pass_one__get_locctr());
  pass_one__insert_to_index_table(pass_one_cb_counter, pass_one__get_locctr());

  // START
  if (pass_one_cb_counter == 1) {
    gboolean program_start = pass_one__find_start_symbol(opcode, data);
    if (program_start == FALSE) {
      pass_one__update_locctr(0);
      pass_one__update_starting_address(0);
    }
    return;
  }

  // END
  pass_one__find_end_symbol(opcode);

  // Increment LOCCTR
  pass_one__increment_locctr(opcode, data);
}

static void pass_one__get_token_from_split_string(gchar **split_string,
                                                  gchar *label, gchar *opcode,
                                                  gchar *data) {
  guint split_length = g_strv_length(split_string);
  // g_print("split_length: %d\r\n", split_length);

  switch (split_length) {
  case 3:
    // Will be label, opcode and data
    strcpy(label, split_string[0]);
    strcpy(opcode, split_string[1]);
    strcpy(data, split_string[2]);
    break;
  case 2:
    // Will be opcode and data
    strcpy(label, "");
    strcpy(opcode, split_string[0]);
    strcpy(data, split_string[1]);
    break;
  case 1:
    // Will be only one opcode
    strcpy(label, "");
    strcpy(opcode, split_string[0]);
    strcpy(data, "");
    break;
  default:
    // Throw an error here
    g_error("[LINE: %d], Token Length %d. Should be [1,3]\r\n",
            pass_one_cb_counter, split_length);
    break;
  }

  // Check if opcode is correct
  gboolean ispresent = load_instruction__get_opcode_from_mnemonic(opcode, NULL);
  if (!ispresent) {
    // g_warning("\"%s\" opcode was not present\r\n", opcode);
  }
}

static void pass_one__extract_data_from_string(gchar *str, gchar *return_str) {
  // ? debugging
  // g_print("%s -> %s\r\n", __FUNCTION__, str);

  guint32 counter = 0;
  gboolean appending = FALSE;
  for (int i = 0; i < strlen(str); i++) {
    // ? debugging
    // g_print("%c : %x\r\n", str[i], str[i]);

    if (g_ascii_isalnum(str[i]) || g_ascii_ispunct(str[i])) {
      return_str[counter] = str[i];
      counter++;
      appending = TRUE;
    } else {
      if (appending == TRUE) {
        return_str[counter] = ' ';
        counter++;
        appending = FALSE;
      }
    }
  }

  // ? debugging
  // g_print("FINAL: %s\r\n", return_str);
}

static gboolean pass_one__find_start_symbol(gchar *opcode, gchar *data) {
  gboolean rval = FALSE;
  if (strcmp(opcode, "START") == 0) {
    guint32 location = g_ascii_strtoull(data, NULL, 16);
    pass_one__update_locctr(location);
    pass_one__update_starting_address(location);
    rval = TRUE;
  }

  return rval;
}

static gboolean pass_one__find_end_symbol(gchar *str) {
  gboolean rval = FALSE;
  if (strcmp(str, "END") == 0) {
    guint32 location = pass_one__get_locctr();
    pass_one__update_end_address(location);
    rval = TRUE;

    // ?? debugging
    // g_print("End Location: %x\r\n", location);
  }

  return rval;
}

static void pass_one__update_symtab(gchar *label) {
  if (strcmp(label, "") == 0) {
    return;
  }

  gboolean ispresent = pass_one__get_symtab_locctr(label, NULL);

  if (ispresent) {
    g_error("Label: \"%s\" redefined\r\n", label);
  } else {
    // Update the value
    guint32 *location = g_new(guint32, 1);
    *location = pass_one__get_locctr();
    pass_one__insert_to_hash_table(strdup(label), location);
  }
}

static void pass_one__write_intermediate_instruction(gchar *label,
                                                     gchar *opcode,
                                                     gchar *data) {
  gchar final_buffer[50] = {0};
  g_snprintf(final_buffer, sizeof(final_buffer) / sizeof(gchar), "%s,%s,%s\r\n",
             label, opcode, data);

  io__write_file(intermediate_channel, final_buffer);
}

// HashTable Functions

static void pass_one__create_hash_table(void) {
  symtab = g_hash_table_new(g_str_hash, g_str_equal);
}

static gboolean pass_one__insert_to_hash_table(gchar *key, guint32 *value) {
  gboolean added = g_hash_table_insert(symtab, key, value);
  // g_print("Added: %d\r\n", added);
  return added;
}

static void pass_one__create_index_table(void) {
  indextab = g_hash_table_new(g_int_hash, g_int_equal);
}

static gboolean pass_one__insert_to_index_table(guint32 key, guint32 value) {
  guint32 *keypointer = g_new(guint32, 1);
  guint32 *valuepointer = g_new(guint32, 1);

  *keypointer = key;
  *valuepointer = value;

  gboolean added = g_hash_table_insert(indextab, keypointer, valuepointer);
  // g_print("Added Key %d : %d\r\n", key, added);
  return added;
}
