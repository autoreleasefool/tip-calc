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

// Status bar offset
#ifdef PBL_COLOR
  #define STATUS_BAR_OFFSET 16
#else
  #define STATUS_BAR_OFFSET 0
#endif

// Input identifiers
#define INPUT_SUBTOTAL_DOLLARS 0
#define INPUT_SUBTOTAL_CENTS 1
#define INPUT_SERVICE 2
#define INPUT_TIP 3
#define TOTAL_INPUTS 4

#define MAX_SUBTOTAL 99999
#define MAX_TIP_PCT 999

// Main UI
static Window *s_main_window;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_24;
static Layer *s_layer_separator;
#ifdef PBL_COLOR
  static StatusBarLayer *s_status_bar;
#endif

// Selection indicators
#ifndef PBL_COLOR
  static InverterLayer *s_inverter_current_input;
  static InverterLayer *s_inverter_subtotal_input;
#else
  static Layer *s_layer_subtotal_line;
  static Layer *s_layer_current_input;
  static Layer *s_layer_current_service;
#endif
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
static void draw_separator(Layer *source_layer, GContext *ctx);
#ifdef PBL_COLOR
  static void draw_subtotal_underline(Layer *source_layer, GContext *ctx);
  static void draw_current_input(Layer *source_layer, GContext *ctx);
  static void draw_current_service(Layer *source_layer, GContext *ctx);
#endif

static void initialise_ui(void) {
  s_main_window = window_create();
  #ifdef PBL_COLOR
    window_set_background_color(s_main_window, GColorCeleste);

    s_layer_current_input = layer_create(GRect(1, 1 + STATUS_BAR_OFFSET, 60, 28));
    layer_set_update_proc(s_layer_current_input, draw_current_input);
    layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_layer_current_input);

    s_layer_current_service = layer_create(GRect(69, 33 + STATUS_BAR_OFFSET, 71, 26));
    layer_set_update_proc(s_layer_current_service, draw_current_service);
    layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_layer_current_service);
  #endif

  // Initialising fonts
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);

  // Initialising separator
  s_layer_separator = layer_create(GRect(5, 93 + STATUS_BAR_OFFSET, 134, 1));
  layer_set_update_proc(s_layer_separator, draw_separator);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_layer_separator);

  //Initialising labels
  s_textlayer_label_subtotal = text_layer_create(GRect(1, 1 + STATUS_BAR_OFFSET, 60, 28));
  text_layer_set_text(s_textlayer_label_subtotal, "Subtotal");
  text_layer_set_text_alignment(s_textlayer_label_subtotal, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_subtotal, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_subtotal);

  s_textlayer_label_service = text_layer_create(GRect(1, 29 + STATUS_BAR_OFFSET, 60, 28));
  text_layer_set_text(s_textlayer_label_service, "Service");
  text_layer_set_text_alignment(s_textlayer_label_service, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_service, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_service);

  s_textlayer_label_tip_pct = text_layer_create(GRect(1, 57 + STATUS_BAR_OFFSET, 60, 28));
  text_layer_set_text(s_textlayer_label_tip_pct, "Tip %");
  text_layer_set_text_alignment(s_textlayer_label_tip_pct, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_tip_pct, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_tip_pct);

  s_textlayer_label_tip_amt = text_layer_create(GRect(1, 96 + STATUS_BAR_OFFSET, 60, 28));
  text_layer_set_text(s_textlayer_label_tip_amt, "Tip $");
  text_layer_set_text_alignment(s_textlayer_label_tip_amt, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_tip_amt, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_tip_amt);

  s_textlayer_label_total = text_layer_create(GRect(1, 124 + STATUS_BAR_OFFSET, 60, 28));
  text_layer_set_text(s_textlayer_label_total, "Total");
  text_layer_set_text_alignment(s_textlayer_label_total, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_label_total, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_label_total);

  // Initialising outputs
  s_textlayer_subtotal = text_layer_create(GRect(70, 1 + STATUS_BAR_OFFSET, 70, 28));
  text_layer_set_text_alignment(s_textlayer_subtotal, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_subtotal, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_subtotal);

  s_textlayer_service = text_layer_create(GRect(70, 29 + STATUS_BAR_OFFSET, 70, 28));
  text_layer_set_text_alignment(s_textlayer_service, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_service, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_service);

  s_textlayer_tip_pct = text_layer_create(GRect(70, 57 + STATUS_BAR_OFFSET, 70, 28));
  text_layer_set_text_alignment(s_textlayer_tip_pct, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_tip_pct, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_tip_pct);

  s_textlayer_tip_amt = text_layer_create(GRect(70, 96 + STATUS_BAR_OFFSET, 70, 28));
  text_layer_set_text_alignment(s_textlayer_tip_amt, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_tip_amt, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_tip_amt);

  s_textlayer_total = text_layer_create(GRect(70, 124 + STATUS_BAR_OFFSET, 70, 28));
  text_layer_set_text_alignment(s_textlayer_total, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_total, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_textlayer_total);

  // Initialising selection indicators
  #ifndef PBL_COLOR
    s_inverter_current_input = inverter_layer_create(GRect(5, 5 + STATUS_BAR_OFFSET, 59, 26));
    layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_inverter_current_input);

    s_inverter_subtotal_input = inverter_layer_create(GRect(69, 5 + STATUS_BAR_OFFSET, 47, 26));
    layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_inverter_subtotal_input);
  #else
    s_layer_subtotal_line = layer_create(GRect(70, 29 + STATUS_BAR_OFFSET, 70, 2));
    layer_set_update_proc(s_layer_subtotal_line, draw_subtotal_underline);
    layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_layer_subtotal_line);

    s_status_bar = status_bar_layer_create();
    layer_add_child(window_get_root_layer(s_main_window), (Layer *)s_status_bar);

    // Setting background of texts to transparent
    text_layer_set_background_color(s_textlayer_label_subtotal, GColorClear);
    text_layer_set_background_color(s_textlayer_label_service, GColorClear);
    text_layer_set_background_color(s_textlayer_label_tip_pct, GColorClear);
    text_layer_set_background_color(s_textlayer_label_tip_amt, GColorClear);
    text_layer_set_background_color(s_textlayer_label_total, GColorClear);

    text_layer_set_background_color(s_textlayer_subtotal, GColorClear);
    text_layer_set_background_color(s_textlayer_service, GColorClear);
    text_layer_set_background_color(s_textlayer_tip_pct, GColorClear);
    text_layer_set_background_color(s_textlayer_tip_amt, GColorClear);
    text_layer_set_background_color(s_textlayer_total, GColorClear);
  #endif
}

static void destroy_ui() {
  window_destroy(s_main_window);

  // Destroying separator
  layer_destroy(s_layer_separator);

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

  #ifndef PBL_COLOR
    // Destroying selection indicators
    inverter_layer_destroy(s_inverter_current_input);
    inverter_layer_destroy(s_inverter_subtotal_input);
  #else
    // Destroying selection indicators
    layer_destroy(s_layer_subtotal_line);
    layer_destroy(s_layer_current_input);
    status_bar_layer_destroy(s_status_bar);
  #endif
}

static void main_window_load(Window *window) {
  update_input_selection();
  update_calc_text();
  register_input_flash_timer();
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
    #ifndef PBL_COLOR
      layer_set_hidden((Layer *)s_inverter_subtotal_input,
          !layer_get_hidden((Layer *)s_inverter_subtotal_input));
    #else
      layer_set_hidden((Layer *)s_layer_subtotal_line,
          !layer_get_hidden((Layer *)s_layer_subtotal_line));
    #endif
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
      #ifndef PBL_COLOR
        layer_set_frame(inverter_layer_get_layer(s_inverter_current_input), GRect(5, 5 + STATUS_BAR_OFFSET, 59, 26));
        layer_set_frame(inverter_layer_get_layer(s_inverter_subtotal_input), GRect(69, 5 + STATUS_BAR_OFFSET, 47, 26));
        layer_set_hidden(inverter_layer_get_layer(s_inverter_subtotal_input), false);
      #else
        text_layer_set_text_color(s_textlayer_label_subtotal, GColorWhite);
        text_layer_set_text_color(s_textlayer_label_tip_pct, GColorBlack);
        layer_set_frame(s_layer_current_input, GRect(5, 5 + STATUS_BAR_OFFSET, 59, 26));
        layer_set_frame(s_layer_subtotal_line, GRect(70, 29 + STATUS_BAR_OFFSET, 48, 2));
        layer_set_hidden(s_layer_subtotal_line, false);
      #endif
      break;
    case INPUT_SUBTOTAL_CENTS:
      #ifndef PBL_COLOR
        layer_set_frame(inverter_layer_get_layer(s_inverter_current_input), GRect(5, 5 + STATUS_BAR_OFFSET, 59, 26));
        layer_set_frame(inverter_layer_get_layer(s_inverter_subtotal_input), GRect(120, 5 + STATUS_BAR_OFFSET, 20, 26));
        layer_set_hidden(inverter_layer_get_layer(s_inverter_subtotal_input), false);
      #else
        layer_set_frame(s_layer_subtotal_line, GRect(120, 29 + STATUS_BAR_OFFSET, 20, 2));
        layer_set_hidden(s_layer_subtotal_line, false);
      #endif
      break;
    case INPUT_SERVICE:
      #ifndef PBL_COLOR
        layer_set_frame(inverter_layer_get_layer(s_inverter_current_input), GRect(5, 33 + STATUS_BAR_OFFSET, 59, 26));
        layer_set_hidden(inverter_layer_get_layer(s_inverter_subtotal_input), true);
      #else
        text_layer_set_text_color(s_textlayer_label_service, GColorWhite);
        text_layer_set_text_color(s_textlayer_label_subtotal, GColorBlack);
        layer_set_frame(s_layer_current_input, GRect(5, 33 + STATUS_BAR_OFFSET, 59, 26));
        layer_set_hidden(s_layer_subtotal_line, true);
      #endif
      break;
    case INPUT_TIP:
      #ifndef PBL_COLOR
        layer_set_frame(inverter_layer_get_layer(s_inverter_current_input), GRect(5, 61 + STATUS_BAR_OFFSET, 59, 26));
        layer_set_hidden(inverter_layer_get_layer(s_inverter_subtotal_input), true);
      #else
        text_layer_set_text_color(s_textlayer_label_tip_pct, GColorWhite);
        text_layer_set_text_color(s_textlayer_label_service, GColorBlack);
        layer_set_frame(s_layer_current_input, GRect(5, 61 + STATUS_BAR_OFFSET, 59, 26));
        layer_set_hidden(s_layer_subtotal_line, true);
      #endif
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
      #ifdef PBL_COLOR

      #endif
      break;
    case SERVICE_AVG_ID:
      tip_pct = g_tip_pct_avg;
      snprintf(s_service_text, sizeof(s_service_text), "%s %s", SERVICE_AVG_VALUE, SMILE);
      #ifdef PBL_COLOR

      #endif
      break;
    case SERVICE_POOR_ID:
      tip_pct = g_tip_pct_poor;
      snprintf(s_service_text, sizeof(s_service_text), "%s %s", SERVICE_POOR_VALUE, FROWN);
      #ifdef PBL_COLOR

      #endif
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

static void draw_separator(Layer *source_layer, GContext *ctx) {
  GPoint p0 = GPoint(0, 0);
  GPoint p1 = GPoint(134, 0);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);
}

#ifdef PBL_COLOR
  static void draw_subtotal_underline(Layer *source_layer, GContext *ctx) {
    GPoint p0 = GPoint(0, 0);
    GPoint p1 = GPoint(144, 0);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_line(ctx, p0, p1);
  }

  static void draw_current_input(Layer *source_layer, GContext *ctx) {
    graphics_context_set_fill_color(ctx, GColorBlueMoon);
    graphics_fill_rect(ctx, GRect(0, 0, 59, 26), 0, GCornerNone);
  }

  static void draw_current_service(Layer *source_layer, GContext *ctx) {
    switch (g_service_selection) {
      case SERVICE_GREAT_ID:
        graphics_context_set_fill_color(ctx, GColorMintGreen);
        break;
      case SERVICE_AVG_ID:
        graphics_context_set_fill_color(ctx, GColorBabyBlueEyes);
        break;
      case SERVICE_POOR_ID:
        graphics_context_set_fill_color(ctx, GColorMelon);
        break;
    }
    graphics_fill_rect(ctx, GRect(0, 0, 71, 26), 0, GCornerNone);
  }
#endif

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch (s_current_input_selection) {
    case INPUT_SUBTOTAL_DOLLARS:
      g_subtotal_cents += 100;
      if (g_subtotal_cents > MAX_SUBTOTAL)
        g_subtotal_cents = MAX_SUBTOTAL;
      break;
    case INPUT_SUBTOTAL_CENTS:
      g_subtotal_cents++;
      if (g_subtotal_cents > MAX_SUBTOTAL)
        g_subtotal_cents = MAX_SUBTOTAL;
      break;
    case INPUT_SERVICE:
      g_service_selection++;
      if (g_service_selection == TOTAL_SERVICE_SELECTIONS)
        g_service_selection = 0;
      #ifdef PBL_COLOR
        layer_mark_dirty(s_layer_current_service);
      #endif
      break;
    case INPUT_TIP:
      switch (g_service_selection) {
        case SERVICE_GREAT_ID:
          g_tip_pct_great++;
          if (g_tip_pct_great > MAX_TIP_PCT)
            g_tip_pct_great = MAX_TIP_PCT;
          break;
        case SERVICE_AVG_ID:
          g_tip_pct_avg++;
          if (g_tip_pct_avg > MAX_TIP_PCT)
            g_tip_pct_avg = MAX_TIP_PCT;
          break;
        case SERVICE_POOR_ID:
          g_tip_pct_poor++;
          if (g_tip_pct_poor > MAX_TIP_PCT)
            g_tip_pct_poor = MAX_TIP_PCT;
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
