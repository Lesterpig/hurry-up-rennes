#include "hurry.h"

#define TIMES_CELL_HEIGHT 22

static int current_stop_id;
static Window *s_window;
static MenuLayer *s_menu_layer;

// Callbacks for menu dynamic definition

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return data_nb_stop_times < 0 ? 1 : data_nb_stop_times;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  char s_time_text[] = "00:00";
  time_t s_time;
  time(&s_time);
  strftime(s_time_text, sizeof(s_time_text), "%H:%M", localtime(&s_time));

  char s_title[30];
  strncpy(s_title, data_stops[current_stop_id], 24);
  strcat(s_title, " ");
  strcat(s_title, s_time_text);

  menu_cell_basic_header_draw(ctx, cell_layer, s_title);
}


static int16_t menu_layer_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return TIMES_CELL_HEIGHT;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  GRect bounds = layer_get_bounds(cell_layer);

  if (data_nb_stop_times > 0) {

    int r = cell_index->row;

    GRect frame_line        = GRect(0, 0, 26, TIMES_CELL_HEIGHT);
    GRect frame_destination = GRect(30, 0, bounds.size.w - 55, TIMES_CELL_HEIGHT);
    GRect frame_time        = GRect(bounds.size.w - 25, 0, 25, TIMES_CELL_HEIGHT);

    graphics_draw_text(ctx, data_stop_times[r].line, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), frame_line, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    graphics_draw_line(ctx, GPoint(27, 0), GPoint(27, TIMES_CELL_HEIGHT));

    graphics_draw_text(ctx, data_stop_times[r].direction, fonts_get_system_font(FONT_KEY_GOTHIC_18), frame_destination, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

    graphics_draw_text(ctx, data_stop_times[r].time, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), frame_time, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);

  } else if (data_nb_stop_times == -1) {
    graphics_draw_text(ctx, "Loading...", fonts_get_system_font(FONT_KEY_GOTHIC_18), bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  } else if (data_nb_stop_times == -2) {
    graphics_draw_text(ctx, "No bus.", fonts_get_system_font(FONT_KEY_GOTHIC_18), bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  }

}

static void menu_long_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  ask_for_bookmark(current_stop_id);
  static const uint32_t const segments[] = { 50, 200, 50 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = 3
  };
  vibes_enqueue_custom_pattern(pat);
}

static void menu_short_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  ask_for_stop_times(current_stop_id);
}

// Window initialization

static void window_load(Window* window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows      = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header       = menu_draw_header_callback,
    .get_cell_height   = menu_layer_get_cell_height_callback,
    .draw_row          = menu_draw_row_callback,
    .select_long_click = menu_long_select_callback,
    .select_click      = menu_short_select_callback
  });

  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

}

static void window_unload(Window* window) {
  menu_layer_destroy(s_menu_layer);
}

void init_times_menu() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
}

void deinit_times_menu() {
  window_destroy(s_window);
}

void times_refresh_ui() {
  menu_layer_reload_data(s_menu_layer);
  vibes_short_pulse();
}

void print_times_menu(int stop_id) {
  current_stop_id = stop_id;
  window_stack_push(s_window, true);
}
