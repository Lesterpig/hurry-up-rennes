#pragma once
#include <pebble.h>

#define STORAGE_BOOK_KEY  0

#define MAX_BOOK_STOPS   10
#define MAX_NEAR_STOPS   10
#define MAX_STOPS        (MAX_BOOK_STOPS + MAX_NEAR_STOPS)
#define MAX_STOP_TIMES   10
#define LINE_NB_LENGTH   3
#define LINE_DIR_LENGTH  20
#define LINE_TIME_LENGTH 3

// Structures

typedef struct stop_time {

  uint8_t accurate;
  char line[LINE_NB_LENGTH + 1];
  char direction[LINE_DIR_LENGTH + 1];
  char time[LINE_TIME_LENGTH + 1];

} stop_time;

// Global variables

// first for bookmarks, seconds for nearest
// -1 for  waiting info from phone
extern int data_nb_stops[2];
extern char* data_stops[MAX_STOPS];

// -1 for waiting info from phone
// 0  for no bus available
extern int data_nb_stop_times;
extern stop_time* data_stop_times;

// Public functions

void init_stops_menu();
void print_stops_menu();
void deinit_stops_menu();

void init_times_menu();
void print_times_menu(int stop_id);
void deinit_times_menu();

void init_timeo_menu();
void print_timeo_menu();
void deinit_timeo_menu();

void init_communications();

void ask_for_stop_times(int stop_id);
void ask_for_bookmark(int stop_id);
void ask_for_timeo(int timeo_id);

void stops_refresh_ui();
void times_refresh_ui();

void set_ready(bool);

