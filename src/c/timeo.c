#include "hurry.h"

#define TIMEO_MIN_ID 1001
#define TIMEO_MAX_ID 6006

static NumberWindow *s_window;
static bool unit_mode;

static void selected_callback(NumberWindow *window, void *data) {
  if (unit_mode) {
    ask_for_timeo(number_window_get_value(window));
    window_stack_pop(true);
  } else {
    number_window_set_step_size(s_window, 1);
    number_window_set_label(s_window, "TIMEO units?");
    unit_mode = true;
  }
}

void init_timeo_menu() {
  s_window = number_window_create("Loading...", (NumberWindowCallbacks) {
    .selected = selected_callback
  }, NULL);
  number_window_set_min(s_window, TIMEO_MIN_ID);
  number_window_set_max(s_window, TIMEO_MAX_ID);
}

void deinit_timeo_menu() {
  number_window_destroy(s_window);
}

void print_timeo_menu() {
  unit_mode = false;
  number_window_set_step_size(s_window, 100);
  number_window_set_label(s_window, "TIMEO hundreds?");
  window_stack_push(number_window_get_window(s_window), true);
}
