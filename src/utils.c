#include <pebble.h>
#include "utils.h"

#define TIP_PCT_GREAT_KEY 0
#define TIP_PCT_AVG_KEY 1
#define TIP_PCT_POOR_KEY 2

// Input values
int g_subtotal_cents = 2000;
int g_service_selection = 1;
int g_tip_pct_great = 22;
int g_tip_pct_avg = 18;
int g_tip_pct_poor = 14;

void load_tip_values(void) {
  if (persist_exists(TIP_PCT_GREAT_KEY))
    g_tip_pct_great = persist_read_int(TIP_PCT_GREAT_KEY);
  if (persist_exists(TIP_PCT_AVG_KEY))
    g_tip_pct_avg = persist_read_int(TIP_PCT_AVG_KEY);
  if (persist_exists(TIP_PCT_POOR_KEY))
    g_tip_pct_poor = persist_read_int(TIP_PCT_POOR_KEY);
}

void save_tip_values(void) {
  persist_write_int(TIP_PCT_GREAT_KEY, g_tip_pct_great);
  persist_write_int(TIP_PCT_AVG_KEY, g_tip_pct_avg);
  persist_write_int(TIP_PCT_POOR_KEY, g_tip_pct_poor);
}
