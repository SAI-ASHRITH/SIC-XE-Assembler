#ifndef SIC_PASS_ONE_H
#define SIC_PASS_ONE_H

#include <glib.h>

/**
 * TODOS
 * 1. Error flag set when duplicate label field found
 * 2.
 *
 */

// TODO, Add filename argument
gboolean pass_one__run(const gchar *filename);

/**
 * @brief Returns true or false if symtab is found
 * *locctr returns our location counter value
 *
 * @param label
 * @param locctr
 * @return gboolean
 */
gboolean pass_one__get_symtab_locctr(gchar *label, guint32 *locctr);

gboolean pass_one__get_index_locctr(guint32 index, guint32 *locctr);

/**
 * @brief
 *
 * @return guint32
 */
guint32 pass_one__get_program_size(void);

#endif
