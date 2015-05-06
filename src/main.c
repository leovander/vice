#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *main_time_layer;
static TextLayer *shadow_time_layer;
static TextLayer *date_layer;
static TextLayer *time_back_layer;
static GFont s_custom_font_40;
static GFont s_custom_font_30;

//Colors
/*
  GColorElectricBlue, GColorShockingPink
*/

char *upcase(char *str)
{
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 0x20;
        }
    }

    return str;
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00 00";
  static char date_buffer[] = "01 JAN";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof(buffer), "%H %M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof(buffer), "%I %M", tick_time);
  }
  
  strftime(date_buffer, sizeof(date_buffer), "%d %b", tick_time);
  upcase(date_buffer);
  
  // Display this time on the TextLayer
  text_layer_set_text(main_time_layer, buffer);
  text_layer_set_text(shadow_time_layer, buffer);
  text_layer_set_text(date_layer, date_buffer);
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  main_time_layer = text_layer_create(GRect(0, 75, 138, 50));
  text_layer_set_background_color(main_time_layer, GColorClear);
  text_layer_set_text_color(main_time_layer, GColorShockingPink);
  text_layer_set_text(main_time_layer, "00 00");

  text_layer_set_font(main_time_layer, s_custom_font_40);
  text_layer_set_text_alignment(main_time_layer, GTextAlignmentCenter);
  
  // Create shadow TextLayer
  shadow_time_layer = text_layer_create(GRect(5, 80, 138, 50));
  text_layer_set_background_color(shadow_time_layer, GColorClear);
  text_layer_set_text_color(shadow_time_layer, GColorBlack);
  text_layer_set_text(shadow_time_layer, "00 00");

  text_layer_set_font(shadow_time_layer, s_custom_font_40);
  text_layer_set_text_alignment(shadow_time_layer, GTextAlignmentCenter);
  
  // Create date TextLayer
  date_layer = text_layer_create(GRect(0, 40, 144, 50));
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(date_layer, GColorCyan);
  text_layer_set_text(date_layer, "01 Jan");

  text_layer_set_font(date_layer, s_custom_font_30);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  
  // Create time background TextLayer
  time_back_layer = text_layer_create(GRect(0, 80, 144, 35));
  text_layer_set_background_color(time_back_layer, GColorCyan);

  // Add it as a children layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_back_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(shadow_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(main_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  fonts_unload_custom_font(s_custom_font_40);
  fonts_unload_custom_font(s_custom_font_30);
  text_layer_destroy(main_time_layer);
  text_layer_destroy(shadow_time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(time_back_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorWhite);
  s_custom_font_40 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BROADWAY_40));
  s_custom_font_30 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BROADWAY_30));

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
