#include <glib.h>

static GTimer *timer;

void timer__start(void) {
  if (timer == NULL) {
    timer = g_timer_new();
  }
}

gdouble timer__get_elapsed(void) {
  gdouble elapsed_time = -1;
  gulong unused_microseconds;

  if (timer != NULL) {
    elapsed_time = g_timer_elapsed(timer, &unused_microseconds);
  }
  return elapsed_time;
}

void timer__reset_time(void) {
  if (timer != NULL) {
    g_timer_start(timer);
  }
}

void timer__destroy(void) {
  if (timer != NULL) {
    g_timer_destroy(timer);
    timer = NULL;
  }
}
