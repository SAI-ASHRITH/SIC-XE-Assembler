#include "load_instruction.h"

#include <gmodule.h>

#include "io.h"

/**
 * CONST DEFNITIONS
 */
#define LOAD_DEBUG 0

/**
 * STATIC DECLARATIONS
 */

// IO
static void load_instruction__load_cb(gchar *str);

// Hash Table
static void load_instruction__create_hash_table(void);
static void load_instruction__special_instructions(void);
static gboolean load_instruction__insert_to_hash_table(gchar *key,
                                                       instruction_t *value);

// TESTER
static void g_hash_foreach_cb(gpointer key, gpointer value, gpointer user_data);

/**
 * STATIC STATE VARIABLES
 */
static GHashTable *instruction_table;

/**
 * FUNCTION
 */
gboolean load_instruction__load(const gchar *filename) {
  // Create the Hash Table to store instructions
  load_instruction__create_hash_table();
  load_instruction__special_instructions();

  // Open the file
  gboolean opened;
  GIOChannel *channel = io__open_file(filename, "r", &opened);

#if LOAD_DEBUG == 1
  g_print("Opened: %d\r\n", opened);
  g_print("Channel: %p\r\n", channel);
#endif

  if (!opened) {
    // TODO, Exit here, FATAL error
    return opened; // false
  }

  // Read data from file and add to hashtable
  io__read_file(channel, load_instruction__load_cb);

  return opened;
}

gboolean
load_instruction__get_opcode_from_mnemonic(const gchar *mnemonic,
                                           instruction_t *return_instruction) {
  // ? debugging
  // g_print("%s\r\n", __FUNCTION__);
  // g_hash_table_foreach(instruction_table, g_hash_foreach_cb, NULL);

  // gpointer value = g_hash_table_lookup(instruction_table, mnemonic);
  // g_print("gpointer: %p\r\n", value);

  gboolean found = FALSE;
  instruction_t *value = g_hash_table_lookup(instruction_table, mnemonic);

  if (value != NULL) {
    found = TRUE;
    if (return_instruction != NULL) {
      *return_instruction = *value; // copy
    }
  }

  // ? debugging
  // g_print("Recv -> Format: %d, Opcode: %x\r\n", value->format,
  // value->opcode);

  return found;
}

/**
 * TESTER
 */
static void g_hash_foreach_cb(gpointer key, gpointer value,
                              gpointer user_data) {
  instruction_t *t = (instruction_t *)value;
  g_print("Key: %s, Value: %x\r\n", key, t->opcode);
}

/**
 * STATIC DEFINITIONS
 */
static void load_instruction__load_cb(gchar *str) {
  // DONE, Parse the string
  gchar **split_string = g_strsplit(str, ",", 3);

  // ? debugging
  // for (int i = 0; i < 3; i++) {
  //   g_print("%d -> %s\r\n", i, split_string[i]);
  // }

  // DONE, Convert to respective numeric format
  guint8 format = (guint8)g_ascii_strtoull(split_string[1], NULL, 10);
  guint8 opcode = (guint8)g_ascii_strtoull(split_string[2], NULL, 16);

  // ? debugging
  // g_print("Format: %s %d 0x%x\r\n", split_string[0], format, opcode);

  // DONE, Add this to static hashtable here
  instruction_t *instruction = g_new(instruction_t, 1);
  instruction->format = format;
  instruction->opcode = opcode;
  gboolean added = load_instruction__insert_to_hash_table(
      g_strdup(split_string[0]), instruction);

  // ? debugging
  // g_print("Added: %d\r\n", added);

#if LOAD_DEBUG == 1
  g_print("----------\r\n");
#endif

  // Free the string
  g_strfreev(split_string);
}

// Hash Table
static void load_instruction__create_hash_table(void) {
  instruction_table = g_hash_table_new(g_str_hash, g_str_equal);
}

static void load_instruction__special_instructions(void) {
  // TODO, Add special values here
}

static gboolean load_instruction__insert_to_hash_table(gchar *key,
                                                       instruction_t *value) {
  gboolean added = g_hash_table_insert(instruction_table, key, value);
#if LOAD_DEBUG == 1
  g_print("Added Key: %s:%d\r\n", key, added);
#endif

  return added;
}
