#include "hurry.h"

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 3

static Window *s_main_window;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_first_menu_items[NUM_FIRST_MENU_ITEMS];

static void stops_mark_dirty() {
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void menu_select_callback(int index, void *ctx) {
  ask_for_stop_times(index);
  print_times_menu(index);
}

static void main_window_load(Window *window) {

  // Although we already defined NUM_FIRST_MENU_ITEMS, you can define
  // an int as such to easily change the order of menu items later
  int num_a_items = 0;

  s_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "Loading...",
    .callback = menu_select_callback,
  };
  s_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "Loading...",
    .callback = menu_select_callback,
  };
  s_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "Loading...",
    .callback = menu_select_callback,
  };

  s_menu_sections[0] = (SimpleMenuSection) {
    .num_items = NUM_FIRST_MENU_ITEMS,
    .items = s_first_menu_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

void main_window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_menu_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

void stops_refresh_ui() {
  int i;
  for(i = 0; i < NB_STOPS; i++) {
    s_first_menu_items[i].title = data_stops_names[i];
  }
  stops_mark_dirty();
}

void print_stops_menu() {
  init();
  app_event_loop();
  deinit();
}
