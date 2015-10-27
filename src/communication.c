#include "hurry.h"

static bool ready = false;

static void save_stops(char* data, int length, bool nearest) {

  int max = (nearest ? MAX_NEAR_STOPS : MAX_BOOK_STOPS);
  int padding = (nearest ? MAX_BOOK_STOPS : 0);
  int i, nb, last, len;

  for (i = 0, nb = 0, last = 0; i < length && nb < max; i++) {

    if (data[i] == '\0') {

      if (data_stops[nb + padding] != NULL) {
        free(data_stops[nb + padding]);
      }

      len = i - last + 1;
      data_stops[nb + padding] = malloc(len * sizeof(char));
      memcpy(data_stops[nb + padding], data + last, len * sizeof(char));

      APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved stop %s - length: %d", data_stops[nb + padding], len);

      nb++;
      last = i + 1;
    }

  }

  data_nb_stops[nearest ? 1 : 0] = nb;
  if (ready) {
    stops_refresh_ui();
  }

}

static void save_stop_times(stop_time* times, int length) {

  data_nb_stop_times = length / sizeof(stop_time);

  if (data_stop_times != NULL) {
    free(data_stop_times);
  }

  if (data_nb_stop_times == 0) {
    return;
  }

  data_stop_times = malloc(data_nb_stop_times * sizeof(stop_time));
  if (data_stop_times == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unable to save stop times!");
    return;
  }

  memcpy(data_stop_times, times, data_nb_stop_times * sizeof(stop_time));

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Saved %d stop times in memory - %d bytes", data_nb_stop_times, length);

  if (ready) {
    times_refresh_ui();
  }

  int i;
  for (i = 0; i < data_nb_stop_times; i++) {

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

  while (t != NULL) {
    switch (t->key) {
      case KEY_STOPS_BOOK:
        save_stops((char *) t->value->data, (int) t->length, false);
        if (t->length <= PERSIST_DATA_MAX_LENGTH) {
          persist_write_data(STORAGE_BOOK_KEY, t->value->data, t->length);
        }
        break;
      case KEY_STOPS_NEAR:
        save_stops((char *) t->value->data, (int) t->length, true);
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

static void send_int(int key, int value) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_int(iter, key, &value, sizeof(int), true);
  dict_write_end(iter);
  app_message_outbox_send();
}

void ask_for_stop_times(int stop_id) {

  // Reset previous data if needed

  if (data_nb_stop_times > 0) {
    free(data_stop_times);
    data_stop_times = NULL;
  }
  data_nb_stop_times = -1;
  send_int(KEY_ASK_STOP, stop_id);
}

void ask_for_bookmark(int stop_id) {
  send_int(KEY_ASK_BOOK, stop_id);
}

void ask_for_timeo(int timeo_id) {
  send_int(KEY_ASK_TIMEO, timeo_id);
}

void init_communications() {
  // Fetch stored bookmarks
  char buffer[PERSIST_DATA_MAX_LENGTH];
  int length;
  length = persist_read_data(STORAGE_BOOK_KEY, buffer, PERSIST_DATA_MAX_LENGTH);

  if (length != E_DOES_NOT_EXIST) {
    save_stops((char *) buffer, length, false);
  }

  // Init callbacks
  app_message_register_inbox_received(inbox_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void set_ready(bool r) {
  ready = r;
}

