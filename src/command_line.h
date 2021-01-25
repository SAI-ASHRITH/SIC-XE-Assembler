#ifndef SIC_COMMAND_LINE_H
#define SIC_COMMAND_LINE_H

#include <glib.h>

void command_line__init(int argc, char **argv);

guint command_line__get_threads();

gchar *command_line__get_filename();
unsigned char *command_line__get_aes_key();

#endif
