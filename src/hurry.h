/*
 * data.h
 *
 * Contains global data structures
 */

#ifndef HURRY_H_
#define HURRY_H_

#include "pebble.h"

#define NB_STOPS         3
#define STOP_LENGTH      50
#define LINE_NB_LENGTH   3
#define LINE_DIR_LENGTH  20
#define LINE_TIME_LENGTH 3

#define KEY_STOPS         0
#define KEY_STOP_TIMES    1
#define KEY_ASK_STOP      2
#define KEY_NO_STOP_TIMES 3

// Structures

typedef struct stop_time {

  uint8_t accurate;
  char line[LINE_NB_LENGTH + 1];
  char direction[LINE_DIR_LENGTH + 1];
  char time[LINE_TIME_LENGTH + 1];

} stop_time;

// Global variables

char data_stops_names[NB_STOPS][STOP_LENGTH];
int data_nb_stop_times;
stop_time* data_stop_times;

// Starter functions

void print_stops_menu();
void print_times_menu(int stop_id);
void init_communications();
void ask_for_stop_times(int stop_id);

// Refresh functions for UI

void stops_refresh_ui();
void times_refresh_ui();

#endif
