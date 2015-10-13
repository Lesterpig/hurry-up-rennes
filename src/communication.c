#include "hurry.h"

static void save_stops(char* data, int length) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "save_stops with length = %d", length);

  int i, nb, last;
  for(i = 0, nb = 0, last = 0; i < length && nb < NB_STOPS; i++) {

    if(data[i] == '\0') {
      strncpy(data_stops_names[nb], data + last, STOP_LENGTH - 1);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved stop #%d: %s", nb, data_stops_names[nb]);
      nb++;
      last = i + 1;
    }

  }

  stops_refresh_ui();

}

static void save_stop_times(stop_time* times, int length) {

  data_nb_stop_times = length / sizeof(stop_time);

  if(data_stop_times != NULL) {
    free(data_stop_times);
  }

  if(data_nb_stop_times == 0) {
    return;
  }

  data_stop_times = malloc(data_nb_stop_times * sizeof(stop_time));
  if(data_stop_times == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unable to save stop times!");
    return;
  }

  memcpy(data_stop_times, times, data_nb_stop_times * sizeof(stop_time));

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved %d stop times in memory - %d bytes", data_nb_stop_times, length);


  times_refresh_ui();

  int i;
  for(i = 0; i < data_nb_stop_times; i++) {

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Line %s to %s : %s [%d]",
      data_stop_times[i].line,
      data_stop_times[i].direction,
      data_stop_times[i].time,
      data_stop_times[i].accurate
    );

  }

}

static void inbox_callback(DictionaryIterator *iterator, void *context) {

  Tuple *t = dict_read_first(iterator);

  while(t != NULL) {
    switch(t->key) {
      case KEY_STOPS:
        save_stops((char *) t->value->data, (int) t->length);
        break;
      case KEY_STOP_TIMES:
        save_stop_times((stop_time*) t->value->data, (int) t->length);
        break;
      case KEY_NO_STOP_TIMES:
        data_nb_stop_times = -2;
        times_refresh_ui();
        break;
    }
    t = dict_read_next(iterator);
  }

}

void ask_for_stop_times(int stop_id) {

  // Reset previous data if needed

  if(data_nb_stop_times > 0) {
    free(data_stop_times);
    data_stop_times = NULL;
  }
  data_nb_stop_times = -1;

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_int(iter, KEY_ASK_STOP, &stop_id, sizeof(int), true);
  dict_write_end(iter);
  app_message_outbox_send();
}

void init_communications() {
  app_message_register_inbox_received(inbox_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}
