/*
 * The MIT License (MIT)
 *
 * TipCalc
 * Copyright (c) 2015 Joseph Roque
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <pebble.h>
#include "utils.h"

#define TIP_PCT_GREAT_KEY 0
#define TIP_PCT_AVG_KEY 1
#define TIP_PCT_POOR_KEY 2
#define CURRENCY_TYPE_KEY 3
#define CURRENCY_SET_KEY 4

// Input values
int g_subtotal_cents = 2000;
int g_service_selection = 1;
int g_tip_pct_great = 22;
int g_tip_pct_avg = 18;
int g_tip_pct_poor = 14;
int g_currency_type = 0;
int g_currency_set = 0;

void load_tip_values(void) {
  if (persist_exists(TIP_PCT_GREAT_KEY))
    g_tip_pct_great = persist_read_int(TIP_PCT_GREAT_KEY);
  if (persist_exists(TIP_PCT_AVG_KEY))
    g_tip_pct_avg = persist_read_int(TIP_PCT_AVG_KEY);
  if (persist_exists(TIP_PCT_POOR_KEY))
    g_tip_pct_poor = persist_read_int(TIP_PCT_POOR_KEY);
  if (persist_exists(CURRENCY_TYPE_KEY))
    g_currency_type = persist_read_int(CURRENCY_TYPE_KEY);
  if (persist_exists(CURRENCY_SET_KEY))
    g_currency_set = persist_read_int(CURRENCY_SET_KEY);
}

void save_tip_values(void) {
  persist_write_int(TIP_PCT_GREAT_KEY, g_tip_pct_great);
  persist_write_int(TIP_PCT_AVG_KEY, g_tip_pct_avg);
  persist_write_int(TIP_PCT_POOR_KEY, g_tip_pct_poor);
  persist_write_int(CURRENCY_TYPE_KEY, g_currency_type);
  persist_write_int(CURRENCY_SET_KEY, g_currency_set);
}

void load_default_values(void) {
  switch (g_currency_type) {
    case DOLLAR:
    case EURO:
    case POUND:
      g_subtotal_cents = 2000;
  }
}

char* get_current_currency_symbol(void) {
  switch (g_currency_type) {
    case EURO:
      return EURO_SYM;
    case POUND:
      return POUND_SYM;
    default:
      return DOLLAR_SYM;
  }
}
