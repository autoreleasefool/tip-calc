#include <pebble.h>
#include "calc_menu.h"

static void init(void) {
  show_calc_menu();
}

static void deinit(void) {
  hide_calc_menu();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
