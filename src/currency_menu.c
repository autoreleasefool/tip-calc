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
#include "calc_menu.h"
#include "currency_menu.h"
#include "utils.h"

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 3

// Main UI
static Window *s_main_window;
static MenuLayer *s_currency_menu;

static void initialise_ui(void) {
  s_main_window = window_create();
  #ifdef PBL_COLOR
    window_set_background_color(s_main_window, GColorCeleste);
  #endif
}

static void destroy_ui() {
  window_destroy(s_main_window);
  menu_layer_destroy(s_currency_menu);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section, void *data) {
  switch (section) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    default:
      return 0;
  }
}

static char* get_header_text(uint16_t section_index) {
  switch (section_index) {
    case 0:
      return "Select a currency";
    default:
      return "";
  }
}

static char* get_row_text(uint16_t section, uint16_t row) {
  switch (section) {
    case 0:
      switch (row) {
        case 0:
          return DOLLAR_SYM;
        case 1:
          return EURO_SYM;
        case 2:
          return POUND_SYM;
        default:
          return "";
      }
    default:
      return "";
  }
}

static char* get_row_subtitle(uint16_t section, uint16_t row) {
  switch (section) {
    case 0:
      switch (row) {
        case 0:
          return "Dollar";
        case 1:
          return "Euro";
        case 2:
          return "Pound";
        default:
          return "";
      }
    default:
      return "";
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, get_header_text(section));
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  uint16_t section = cell_index->section;
  uint16_t row = cell_index->row;
  menu_cell_basic_draw(ctx, cell_layer, get_row_text(section, row), get_row_subtitle(section, row), NULL);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  g_currency_set = 1;
  g_currency_type = cell_index->row;
  if (!g_calc_is_on_stack) {
    load_default_values();
    show_calc_menu();
  }
  hide_currency_menu();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_currency_menu = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_currency_menu, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  menu_layer_set_click_config_onto_window(s_currency_menu, window);
  layer_add_child(window_layer, menu_layer_get_layer(s_currency_menu));
}

static void main_window_unload(Window *window) {
  destroy_ui();
}

void show_currency_menu(void) {
  initialise_ui();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

void hide_currency_menu(void) {
  window_stack_remove(s_main_window, true);
}
