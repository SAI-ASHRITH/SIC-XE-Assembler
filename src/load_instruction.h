#ifndef SIC_LOAD_INSTRUCTION_H
#define SIC_LOAD_INSTRUCTION_H

#include <glib.h>

typedef struct {
  guint8 format;
  guint8 opcode;
} instruction_t;

/**
 * @brief Loads the Instructions from the <filename> file
 * and adds it to a hashtable
 *
 * @param filename
 * @return gboolean
 */
gboolean load_instruction__load(const gchar *filename);

/**
 * @brief Gets the instruction struct (format and opcode) on an input mnemonic
 * Gets it from the internally stored hashtable
 *
 * @param mnemonic
 * @param return_instruction
 */
gboolean
load_instruction__get_opcode_from_mnemonic(const gchar *mnemonic,
                                           instruction_t *return_instruction);

#endif
