#pragma once

#include <gtk/gtk.h>
#include <stddef.h>

void object_file_builder__initialize(guint arr_size);

void object_file_builder__free(void);

void object_file_builder__populate_string_array(size_t i);

void object_file_builder__create_object_file(const gchar *filename);
