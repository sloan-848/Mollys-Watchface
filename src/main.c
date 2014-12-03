#include <pebble.h>

static Window *main_window;
static Layer *canvas_layer;

bool isDark = false;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  //Draws graphic by tiling
  if(isDark){
    graphics_draw_circle(ctx, GPoint(20,20), 20);
    //graphics_draw_bitmap_in_rect(ctx, dark_block, layer_get_bounds(layer));
  } else {
    graphics_draw_circle(ctx, GPoint(40,20), 20);
    //graphics_draw_bitmap_in_rect(ctx, light_block, layer_get_bounds(layer));    
  }
  isDark = !isDark; 
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  //Do stuff
  static char s_time_buffer[16];
  clock_copy_time_string(s_time_buffer, sizeof(s_time_buffer));
  
  time_t current_time = time(NULL);
  localtime(&current_time);
  
  layer_mark_dirty(canvas_layer);
}

static void main_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  //Create Layer
  canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, canvas_layer);
  

  
  //Set update_proc
  layer_set_update_proc(canvas_layer, canvas_update_proc);
}

static void main_window_unload(Window *window){
  //Destroy Layer
  layer_destroy(canvas_layer);
}

static void init(void) {
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(main_window, true);
  
  //Subscribe to minute tick
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit(void) {
  //Destroy main window
  window_destroy(main_window);
  
  //Unsubscribe to minute tick
  tick_timer_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}