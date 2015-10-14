#include "hurry.h"

char data_stops_names[NB_STOPS][STOP_LENGTH];
int data_nb_stop_times = -1;
stop_time* data_stop_times = NULL;

int main(void) {
  init_communications();
  print_stops_menu();
}
