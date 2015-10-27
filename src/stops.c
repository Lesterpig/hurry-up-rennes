#include "hurry.h"

static Window *s_window;
static MenuLayer *s_menu_layer;

static GBitmap *s_add_icon;
static GBitmap *s_bookmark_icon;
static GBitmap *s_near_icon;

static int nb_for_section(int section) {
  if (section > 2) {
    return 0;
  }
  if (data_nb_stops[section] < 0) {
    return 1 + (section == 0 ? 1 : 0);
  } else {
    return data_nb_stops[section] + (section == 0 ? 1 : 0);
  }
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return (nb_for_section(0) > 0 ? 1 : 0) + (nb_for_section(1) > 0 ? 1 : 0);
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  if (section_index == 0 && nb_for_section(0) > 0) {
    return nb_for_section(0);
  } else {
    return nb_for_section(1);
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  if (section_index == 0 && nb_for_section(0) > 0) {
    menu_cell_basic_header_draw(ctx, cell_layer, "Bookmarks");
  } else {
    menu_cell_basic_header_draw(ctx, cell_layer, "Nearest");
  }
}


static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  int index = cell_index->row;
  int section = 0;
  if (cell_index->section == 1 || nb_for_section(0) == 0) {
    index += MAX_BOOK_STOPS;
    section = 1;
  }

  if (data_nb_stops[section] == -1 && cell_index->row == 0) {
    menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
  } else if (section == 0 && index == nb_for_section(0) - 1) {
    menu_cell_basic_draw(ctx, cell_layer, "Add bookmark", "TIMEO code", s_add_icon);
  } else {
    GBitmap* icon = (section == 0 ? s_bookmark_icon : s_near_icon);
    menu_cell_basic_draw(ctx, cell_layer, data_stops[index], NULL, icon);
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  int index = cell_index->row;
  int section = 0;
  if (cell_index->section == 1 || nb_for_section(0) == 0) {
    index += MAX_BOOK_STOPS;
    section = 1;
  } else if (index == nb_for_section(0) - 1) {
    print_timeo_menu();
    return;
  }

  if(data_nb_stops[section] <= 0) {
    return;
  }

  ask_for_stop_times(index);
  print_times_menu(index);
}


static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_add_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ADD_ICON);
  s_bookmark_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BOOKMARK_ICON);
  s_near_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEAR_ICON);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_sections  = menu_get_num_sections_callback,
    .get_num_rows      = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header       = menu_draw_header_callback,
    .draw_row          = menu_draw_row_callback,
    .select_click      = menu_select_callback,
  });

  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  gbitmap_destroy(s_add_icon);
  gbitmap_destroy(s_bookmark_icon);
  gbitmap_destroy(s_near_icon);
}

void init_stops_menu() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void deinit_stops_menu() {
  window_destroy(s_window);
}

void stops_refresh_ui() {
  menu_layer_reload_data(s_menu_layer);
}

void print_stops_menu() {
  window_stack_push(s_window, true);
}
