#include "io.h"

GIOChannel *io__open_file(const gchar *filename, const gchar *mode,
                          gboolean *return_opened) {
  *return_opened = TRUE;
  GError *error = NULL;

  GIOChannel *channel = g_io_channel_new_file(filename, mode, &error);
  if (error != NULL) {
    g_printerr("[%s] -> %s\r\n", __FUNCTION__, error->message);
    *return_opened = FALSE;
  }

  return channel;
}

gboolean io__close_file(GIOChannel *channel, gboolean flush) {
  GError *error = NULL;
  gboolean rval = TRUE;
  g_io_channel_shutdown(channel, flush, &error);
  if (error != NULL) {
    g_printerr("%s -> %s\r\n", __FUNCTION__, error->message);
    rval = FALSE;
  }

  return rval;
}

void io__read_file(GIOChannel *channel, io__read_file_cb read_file_cb) {
  // Make this 0 and NULL else doesnt work
  GString buffer = {0};
  GError *error = NULL;

  GIOStatus status;
  while ((status = g_io_channel_read_line_string(
              channel, &buffer, NULL, &error)) == G_IO_STATUS_NORMAL) {

    // Strip whitespaces
    g_strstrip(buffer.str);

    if (read_file_cb != NULL) {
      read_file_cb(buffer.str);
    }

    // END
  }
}

void io__write_file(GIOChannel *channel, const gchar *buf) {
  gsize bytes_written = 0;
  GError *error = NULL;
  GIOStatus status = g_io_channel_write_chars(channel, buf, strlen(buf),
                                              &bytes_written, &error);
  if (error != NULL) {
    g_printerr("%s -> %s\r\n", __FUNCTION__, error->message);
  }
}
