#ifndef SIC_IO_H
#define SIC_IO_H

#include <glib.h>

typedef void (*io__read_file_cb)(gchar *);

/**
 * @brief Opens a file and returns a channel to work with
 * Call `io__close_file` once you are done
 *
 * @param filename
 * @param mode
 * @param return_opened
 * @return GIOChannel*
 *
 * ! NOTE `return_opened` is NOT NULLABLE
 */
GIOChannel *io__open_file(const gchar *filename, const gchar *mode,
                          gboolean *return_opened);

/**
 * @brief Closes a file with the Channel provided from `io__open_file`
 *
 * @param channel
 * @param flush (Notifies the system if it needs to flush data to the file)
 * @return gboolean
 */
gboolean io__close_file(GIOChannel *channel, gboolean flush);

/**
 * @brief Reads a file per line (newline character)
 * On every newline character received it calls `read_file_cb` function
 * ! NOTE: `read_file_cb` function CAN be NULLABLE
 *
 * @param channel
 * @param read_file_cb @Nullable
 */
void io__read_file(GIOChannel *channel, io__read_file_cb read_file_cb);

/**
 * @brief Writes to file with the buffer provided
 * ! Bug, only completes writing to the file when `io__close_file` is called
 *
 * @param channel
 * @param buf
 */
void io__write_file(GIOChannel *channel, const gchar *buf);

#endif
