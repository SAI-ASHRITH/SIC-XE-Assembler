#ifndef SIC_PASS_TWO_H
#define SIC_PASS_TWO_H

#include <glib.h>

gboolean pass_two__run(const gchar *filename);
void pass_two__clean(void);

guint pass_two__get_instruction_array_size(void);

/**
 * @brief Returns parsed object string out
 * ! Allocated memory, call `g_free` after use
 *
 * @param index
 * @return gchar*
 *
 * ! Return value can be null (check before input into string array)
 */
gchar *pass_two__parse_instruction_at_index(guint32 index, guint32 *locctr);

#endif
