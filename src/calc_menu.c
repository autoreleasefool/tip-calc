#include <pebble.h>
#include "calc_menu.h"

// Main UI
static Window *s_main_window;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_24;

// Labels
static TextLayer *s_textlayer_label_subtotal;
static TextLayer *s_textlayer_label_service;
static TextLayer *s_textlayer_label_tip_pct;
static TextLayer *s_textlayer_label_tip_amt;
static TextLayer *s_textlayer_label_total;

// Text output
static TextLayer *s_textlayer_subtotal;
static TextLayer *s_textlayer_service;
static TextLayer *s_textlayer_tip_pct;
static TextLayer *s_textlayer_tip_amt;
static TextLayer *s_textlayer_total;

static void initialise_ui(void) {
  s_main_window = window_create();
  /*#ifndef PBL_SDK_3
    window_set_fullscreen(s_main_window, true);
  #endif*/

  // Initialising fonts
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);

  //Initialising labels
  s_textlayer_label_subtotal = text_layer_create(GRect(1, 1, 60, 26));
  text_layer_set_text(s_textlayer_label_subtotal, "Subtotal");
  text_layer_set_text_alignment(s_textlayer_label_subtotal, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_subtotal, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_subtotal);

  s_textlayer_label_service = text_layer_create(GRect(1, 27, 60, 26));
  text_layer_set_text(s_textlayer_label_service, "Service");
  text_layer_set_text_alignment(s_textlayer_label_service, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_service, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_service);

  s_textlayer_label_tip_pct = text_layer_create(GRect(1, 53, 60, 26));
  text_layer_set_text(s_textlayer_label_tip_pct, "Tip %");
  text_layer_set_text_alignment(s_textlayer_label_tip_pct, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_tip_pct, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_tip_pct);

  s_textlayer_label_tip_amt = text_layer_create(GRect(1, 100, 60, 26));
  text_layer_set_text(s_textlayer_label_tip_amt, "Tip $");
  text_layer_set_text_alignment(s_textlayer_label_tip_amt, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_tip_amt, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_tip_amt);

  s_textlayer_label_total = text_layer_create(GRect(1, 126, 60, 26));
  text_layer_set_text(s_textlayer_label_total, "Total");
  text_layer_set_text_alignment(s_textlayer_label_total, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_total, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_total);

  // Initialising outputs
  s_textlayer_subtotal = text_layer_create(GRect(70, 1, 70, 28));
  text_layer_set_text(s_textlayer_subtotal, "$20.00");
  text_layer_set_text_alignment(s_textlayer_subtotal, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_subtotal, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_subtotal);

  s_textlayer_service = text_layer_create(GRect(70, 29, 70, 28));
  text_layer_set_text(s_textlayer_service, "Avg");
  text_layer_set_text_alignment(s_textlayer_service, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_service, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_service);

  s_textlayer_tip_pct = text_layer_create(GRect(70, 57, 70, 28));
  text_layer_set_text(s_textlayer_tip_pct, "15%");
  text_layer_set_text_alignment(s_textlayer_tip_pct, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_tip_pct, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_tip_pct);

  s_textlayer_tip_amt = text_layer_create(GRect(70, 96, 70, 28));
  text_layer_set_text(s_textlayer_tip_amt, "$3.00");
  text_layer_set_text_alignment(s_textlayer_tip_amt, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_tip_amt, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_tip_amt);

  s_textlayer_total = text_layer_create(GRect(70, 124, 70, 28));
  text_layer_set_text(s_textlayer_total, "$23.00");
  text_layer_set_text_alignment(s_textlayer_total, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_total, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_total);
}

static void destroy_ui() {
  window_destroy(s_main_window);

  // Destroying labels
  text_layer_destroy(s_textlayer_label_subtotal);
  text_layer_destroy(s_textlayer_label_service);
  text_layer_destroy(s_textlayer_label_tip_pct);
  text_layer_destroy(s_textlayer_label_tip_amt);
  text_layer_destroy(s_textlayer_label_total);

  // Destroying outputs
  text_layer_destroy(s_textlayer_subtotal);
  text_layer_destroy(s_textlayer_service);
  text_layer_destroy(s_textlayer_tip_pct);
  text_layer_destroy(s_textlayer_tip_amt);
  text_layer_destroy(s_textlayer_total);
}

static void main_window_load(Window *window) {

}

static void main_window_unload(Window *window) {
  destroy_ui();
}

void show_calc_menu(void) {
  initialise_ui();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

void hide_calc_menu(void) {
  window_stack_remove(s_main_window, true);
}
