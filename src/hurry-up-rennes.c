#include "hurry.h"

// Define global variables
int data_nb_stops[2] = { 0, -1 };
char* data_stops[MAX_STOPS] = { NULL };

int data_nb_stop_times = -1;
stop_time* data_stop_times = NULL;

int main(void) {
  init_communications();
  init_stops_menu();
  init_times_menu();

  print_stops_menu();
  app_event_loop();

  deinit_times_menu();
  deinit_stops_menu();
}
