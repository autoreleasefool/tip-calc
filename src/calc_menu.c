#include <pebble.h>
#include "calc_menu.h"
#include "utils.h"

// Service identifiers
#define SERVICE_GREAT_ID 2
#define SERVICE_GREAT_VALUE "Great"
#define SERVICE_AVG_ID 1
#define SERVICE_AVG_VALUE "Avg"
#define SERVICE_POOR_ID 0
#define SERVICE_POOR_VALUE "Poor"
#define TOTAL_SERVICE_SELECTIONS 3

// Emojis
#define BIG_SMILE "\U0001F604"
#define SMILE "\U0001F60A"
#define FROWN "\U0001F629"

// Input identifiers
#define INPUT_SUBTOTAL_DOLLARS 0
#define INPUT_SUBTOTAL_CENTS 1
#define INPUT_SERVICE 2
#define INPUT_TIP 3
#define TOTAL_INPUTS 4

#define MAX_SUBTOTAL 99999

// Main UI
static Window *s_main_window;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_24;

// Selection indicators
static InverterLayer *s_inverter_current_input;
static InverterLayer *s_inverter_subtotal_input;
static AppTimer *s_input_flash_timer;

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

// Control values
static int s_current_input_selection = 0;

// Output values
static char s_subtotal_text[30] = "";
static char s_service_text[30] = "";
static char s_tip_pct_text[30] = "";
static char s_tip_amt_text[30] = "";
static char s_total_text[30] = "";

static void register_input_flash_timer(void);
static void unregister_input_flash_timer(void);
static void update_input_flash(void *data);
static void update_input_selection(void);
static void update_calc_text(void);

static void initialise_ui(void) {
  s_main_window = window_create();

  // Initialising fonts
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);

  //Initialising labels
  s_textlayer_label_subtotal = text_layer_create(GRect(1, 1, 60, 28));
  text_layer_set_text(s_textlayer_label_subtotal, "Subtotal");
  text_layer_set_text_alignment(s_textlayer_label_subtotal, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_subtotal, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_subtotal);

  s_textlayer_label_service = text_layer_create(GRect(1, 29, 60, 28));
  text_layer_set_text(s_textlayer_label_service, "Service");
  text_layer_set_text_alignment(s_textlayer_label_service, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_service, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_service);

  s_textlayer_label_tip_pct = text_layer_create(GRect(1, 57, 60, 28));
  text_layer_set_text(s_textlayer_label_tip_pct, "Tip %");
  text_layer_set_text_alignment(s_textlayer_label_tip_pct, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_tip_pct, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_tip_pct);

  s_textlayer_label_tip_amt = text_layer_create(GRect(1, 96, 60, 28));
  text_layer_set_text(s_textlayer_label_tip_amt, "Tip $");
  text_layer_set_text_alignment(s_textlayer_label_tip_amt, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_tip_amt, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_tip_amt);

  s_textlayer_label_total = text_layer_create(GRect(1, 124, 60, 28));
  text_layer_set_text(s_textlayer_label_total, "Total");
  text_layer_set_text_alignment(s_textlayer_label_total, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_total, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_total);

  // Initialising outputs
  s_textlayer_subtotal = text_layer_create(GRect(70, 1, 70, 28));
  text_layer_set_text_alignment(s_textlayer_subtotal, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_subtotal, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_subtotal);

  s_textlayer_service = text_layer_create(GRect(70, 29, 70, 28));
  text_layer_set_text_alignment(s_textlayer_service, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_service, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_service);

  s_textlayer_tip_pct = text_layer_create(GRect(70, 57, 70, 28));
  text_layer_set_text_alignment(s_textlayer_tip_pct, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_tip_pct, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_tip_pct);

  s_textlayer_tip_amt = text_layer_create(GRect(70, 96, 70, 28));
  text_layer_set_text_alignment(s_textlayer_tip_amt, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_tip_amt, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_tip_amt);

  s_textlayer_total = text_layer_create(GRect(70, 124, 70, 28));
  text_layer_set_text_alignment(s_textlayer_total, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_total, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_total);

  // Initialising selection indicators
  s_inverter_current_input = inverter_layer_create(GRect(1, 1, 60, 28));
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_inverter_current_input);
  s_inverter_subtotal_input = inverter_layer_create(GRect(70, 1, 70, 28));
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_inverter_subtotal_input);
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

  // Destroying selection indicators
  inverter_layer_destroy(s_inverter_current_input);
  inverter_layer_destroy(s_inverter_subtotal_input);
}

static void main_window_load(Window *window) {
  update_input_selection();
  update_calc_text();
}

static void main_window_unload(Window *window) {
  destroy_ui();
}

static void register_input_flash_timer(void) {
  if (s_input_flash_timer == NULL) {
    s_input_flash_timer = app_timer_register(500, (AppTimerCallback)update_input_flash, NULL);
  }
}

static void unregister_input_flash_timer(void) {
  if (s_input_flash_timer != NULL) {
    app_timer_cancel(s_input_flash_timer);
    s_input_flash_timer = NULL;
  }
}

static void update_input_flash(void *data) {
  if (s_current_input_selection == INPUT_SUBTOTAL_CENTS
      || s_current_input_selection == INPUT_SUBTOTAL_DOLLARS) {
    layer_set_hidden((Layer *)s_inverter_subtotal_input,
        !layer_get_hidden((Layer *)s_inverter_subtotal_input));
    s_input_flash_timer = app_timer_register(500, (AppTimerCallback)update_input_flash, NULL);
  } else {
    if (s_input_flash_timer != NULL) {
      app_timer_cancel(s_input_flash_timer);
      s_input_flash_timer = NULL;
    }
  }
}

static void update_input_selection(void) {
  switch (s_current_input_selection) {
    case INPUT_SUBTOTAL_DOLLARS:
      layer_set_frame(inverter_layer_get_layer(s_inverter_current_input), GRect(5, 5, 59, 26));
      layer_set_frame(inverter_layer_get_layer(s_inverter_subtotal_input), GRect(69, 5, 47, 26));
      layer_set_hidden(inverter_layer_get_layer(s_inverter_subtotal_input), false);
      break;
    case INPUT_SUBTOTAL_CENTS:
      layer_set_frame(inverter_layer_get_layer(s_inverter_current_input), GRect(5, 5, 59, 26));
      layer_set_frame(inverter_layer_get_layer(s_inverter_subtotal_input), GRect(120, 5, 20, 26));
      layer_set_hidden(inverter_layer_get_layer(s_inverter_subtotal_input), false);
      break;
    case INPUT_SERVICE:
      layer_set_frame(inverter_layer_get_layer(s_inverter_current_input), GRect(5, 33, 59, 26));
      layer_set_hidden(inverter_layer_get_layer(s_inverter_subtotal_input), true);
      break;
    case INPUT_TIP:
      layer_set_frame(inverter_layer_get_layer(s_inverter_current_input), GRect(5, 61, 59, 26));
      layer_set_hidden(inverter_layer_get_layer(s_inverter_subtotal_input), true);
      break;
  }
}

static void update_calc_text(void) {
  // Formatting subtotal
  int subtotal_dollars = g_subtotal_cents / 100;
  int subtotal_cents = g_subtotal_cents % 100;
  snprintf(s_subtotal_text, sizeof(s_subtotal_text), "$%d.%02d", subtotal_dollars, subtotal_cents);

  // Formatting service
  int tip_pct = 0;
  switch (g_service_selection) {
    case SERVICE_GREAT_ID:
      tip_pct = g_tip_pct_great;
      snprintf(s_service_text, sizeof(s_service_text), "%s %s", SERVICE_GREAT_VALUE, BIG_SMILE);
      break;
    case SERVICE_AVG_ID:
      tip_pct = g_tip_pct_avg;
      snprintf(s_service_text, sizeof(s_service_text), "%s %s", SERVICE_AVG_VALUE, SMILE);
      break;
    case SERVICE_POOR_ID:
      tip_pct = g_tip_pct_poor;
      snprintf(s_service_text, sizeof(s_service_text), "%s %s", SERVICE_POOR_VALUE, FROWN);
      break;
  }

  // Formatting tip percentage
  snprintf(s_tip_pct_text, sizeof(s_tip_pct_text), "%d%%", tip_pct);

  // Formating tip dollar value
  int tip_amt_raw = (int) (g_subtotal_cents * (tip_pct / 100.0f));
  int tip_amt_dollars = tip_amt_raw / 100;
  int tip_amt_cents = tip_amt_raw % 100;
  snprintf(s_tip_amt_text, sizeof(s_tip_amt_text), "$%d.%02d", tip_amt_dollars, tip_amt_cents);

  // Formatting total
  int total_raw = tip_amt_raw + g_subtotal_cents;
  int total_dollars = total_raw / 100;
  int total_cents = total_raw % 100;
  snprintf(s_total_text, sizeof(s_total_text), "$%d.%02d", total_dollars, total_cents);

  text_layer_set_text(s_textlayer_subtotal, s_subtotal_text);
  text_layer_set_text(s_textlayer_service, s_service_text);
  text_layer_set_text(s_textlayer_tip_pct, s_tip_pct_text);
  text_layer_set_text(s_textlayer_tip_amt, s_tip_amt_text);
  text_layer_set_text(s_textlayer_total, s_total_text);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch (s_current_input_selection) {
    case INPUT_SUBTOTAL_DOLLARS:
      g_subtotal_cents += 100;
      if (g_subtotal_cents > MAX_SUBTOTAL)
        g_subtotal_cents = MAX_SUBTOTAL;
      break;
    case INPUT_SUBTOTAL_CENTS:
      g_subtotal_cents++;
      break;
    case INPUT_SERVICE:
      g_service_selection++;
      if (g_service_selection == TOTAL_SERVICE_SELECTIONS)
        g_service_selection = 0;
      break;
    case INPUT_TIP:
      switch (g_service_selection) {
        case SERVICE_GREAT_ID:
          g_tip_pct_great++;
          break;
        case SERVICE_AVG_ID:
          g_tip_pct_avg++;
          break;
        case SERVICE_POOR_ID:
          g_tip_pct_poor++;
      }
      break;
  }

  update_calc_text();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch (s_current_input_selection) {
    case INPUT_SUBTOTAL_DOLLARS:
      g_subtotal_cents -= 100;
      if (g_subtotal_cents < 0)
        g_subtotal_cents = 0;
      break;
    case INPUT_SUBTOTAL_CENTS:
      g_subtotal_cents--;
      if (g_subtotal_cents < 0)
        g_subtotal_cents = 0;
      break;
    case INPUT_SERVICE:
      g_service_selection--;
      if (g_service_selection < 0)
        g_service_selection = TOTAL_SERVICE_SELECTIONS - 1;
      break;
    case INPUT_TIP:
      switch (g_service_selection) {
        case SERVICE_GREAT_ID:
          g_tip_pct_great--;
          if (g_tip_pct_great < 0)
            g_tip_pct_great = 0;
          break;
        case SERVICE_AVG_ID:
          g_tip_pct_avg--;
          if (g_tip_pct_avg < 0)
            g_tip_pct_avg = 0;
          break;
        case SERVICE_POOR_ID:
          g_tip_pct_poor--;
          if (g_tip_pct_poor < 0)
            g_tip_pct_poor = 0;
      }
      break;
  }

  update_calc_text();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_current_input_selection++;
  if (s_current_input_selection == TOTAL_INPUTS)
    s_current_input_selection = 0;

  switch (s_current_input_selection) {
    case INPUT_SUBTOTAL_DOLLARS:
    case INPUT_SUBTOTAL_CENTS:
      register_input_flash_timer();
      break;
    case INPUT_SERVICE:
    case INPUT_TIP:
      unregister_input_flash_timer();
      break;
  }

  update_input_selection();
}

static void click_config_provider(void *context) {
  // Register click handlers
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 30, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 30, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

void show_calc_menu(void) {
  initialise_ui();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_stack_push(s_main_window, true);
}

void hide_calc_menu(void) {
  window_stack_remove(s_main_window, true);
}
