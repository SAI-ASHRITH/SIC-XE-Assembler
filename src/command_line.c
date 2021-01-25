#include "command_line.h"

#include <gtk/gtk.h>

/**
 * STATIC FUNCTIONS
 */
static gboolean filename_cb(const gchar *option_name, const gchar *value,
                            gpointer data, GError **error);
static gboolean key_cb(const gchar *option_name, const gchar *value,
                       gpointer data, GError **error);
/**
 * STATIC VARIABLES
 */
static gint thread_num = 1; // default
static gchar filename[50];
static unsigned char key[50] = "qwerty"; // default

static GOptionEntry entries[] = {
    {"filename", 0, 0, G_OPTION_ARG_CALLBACK, filename_cb, "File to Parse",
     "filepath"},
    {"threads", 0, 0, G_OPTION_ARG_INT, &thread_num, "Number of Threads to use",
     "N"},
    {"key", 0, 0, G_OPTION_ARG_CALLBACK, key_cb, "Secure AES Key",
     "secure key"},
    {NULL},
};

/**
 * FUNCTIONS
 */
void command_line__init(int argc, char **argv) {
  // ? debugging
  // g_print("Args: %d\r\n", argc);
  // for (int i = 0; i < argc; i++) {
  //   g_print("%d : %s\r\n", i, argv[i]);
  // }

  GError *error = NULL;
  GOptionContext *context = NULL;

  context = g_option_context_new(NULL);
  g_option_context_add_main_entries(context, entries, NULL);
  g_option_context_add_group(context, gtk_get_option_group(TRUE));

  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_error("Option Parsing failed: %s\r\n", error->message);
  }

  // Should NOT be 0
  if (filename[0] == '\0') {
    g_error("--filename argument is missing\r\n");
  }

  if (thread_num <= 0) {
    g_error("--thread cannot be <= 0\r\n");
  }
}

guint command_line__get_threads() { return thread_num; }
gchar *command_line__get_filename() { return filename; }
unsigned char *command_line__get_aes_key() { return key; }

/**
 * STATIC FUNCTION DEFINITIONS
 */
static gboolean filename_cb(const gchar *option_name, const gchar *value,
                            gpointer data, GError **error) {

  // ? debugging
  g_print("[%s] -> %s\r\n", __FUNCTION__, value);

  strcpy(filename, value);
  return TRUE;
}

static gboolean key_cb(const gchar *option_name, const gchar *value,
                       gpointer data, GError **error) {
  // ? debugging
  g_print("[%s] -> %s\r\n", __FUNCTION__, value);

  memset(key, 0, sizeof(key) / sizeof(unsigned char));
  strcpy(key, value);
  return TRUE;
}
