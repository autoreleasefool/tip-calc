#include <pebble.h>
#include "calc_menu.h"
#include "utils.h"

static void init(void) {
  load_tip_values();
  show_calc_menu();
}

static void deinit(void) {
  save_tip_values();
  hide_calc_menu();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
