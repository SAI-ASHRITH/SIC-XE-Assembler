#include "thread.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

typedef struct {
  size_t run_count;
  size_t instruction_address;
  size_t id;
  size_t total_instructions;
  size_t num_of_threads;
  void (*thread_fx)(size_t);
} thread_instruction_s;

static gboolean thread__cb(gpointer user_data) {
  thread_instruction_s *thread_instruction = (thread_instruction_s *)user_data;
  gboolean ret_value = TRUE;

  thread_instruction->thread_fx(thread_instruction->instruction_address);
  thread_instruction->instruction_address += thread_instruction->num_of_threads;

  if (thread_instruction->instruction_address >=
      thread_instruction->total_instructions) {
    ret_value = FALSE;
  }

  return ret_value;
}

void thread__runner(size_t instruction_size, size_t num_of_threads,
                    void (*thread_fx)(size_t)) {
  thread_instruction_s *thread_instructions =
      g_new(thread_instruction_s, num_of_threads);

  for (size_t i = 0; i < num_of_threads; i++) {
    thread_instructions[i].run_count = 0;
    thread_instructions[i].instruction_address = i;
    thread_instructions[i].total_instructions = instruction_size;
    thread_instructions[i].id = i;
    thread_instructions[i].num_of_threads = num_of_threads;
    thread_instructions[i].thread_fx = thread_fx;

    gdk_threads_add_idle(thread__cb, &thread_instructions[i]);
  }

  while (1) {
    if (gtk_events_pending()) {
      gtk_main_iteration();
    } else {
      g_free(thread_instructions);
      g_print("Done threading\r\n");
      break;
    }
  }
}
