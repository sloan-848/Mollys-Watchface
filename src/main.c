#include <pebble.h>

static Window *main_window;
static Layer *canvas_layer;
static TextLayer *clock_text_layer;
static TextLayer *write_text_layer;
static TextLayer *additional_text_layer;
static TextLayer *comp_text_layer;

static GFont *script_font;

static int init_y = 110;

bool first = true;

static char compliments[6][64] = {"CUTIE", "SWEETIE", "BOO BEAR", "PRINCESS", "BFF", "SMARTIE"};

static char time_buffer[32];
static char text_buffer[128];

void on_animation_stopped(Animation *anim, bool finished, void *context)
{
    //Free the memoery used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
 
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
 
    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
 
    //Start animation!
    animation_schedule((Animation*) anim);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  int hour = tick_time->tm_hour;
  int minute = tick_time->tm_min;
  int seconds = tick_time->tm_sec;

  char hourHex[4] = "";
  char minHex[4] = "";

  if(!clock_is_24h_style()){
    if(hour > 12){
      hour -= 12;
    }
    if(hour == 0){
      hour = 12;
    }
  }

  //Start animation 1 second before hour change
  if((minute == 59) && (seconds == 59))
  {
    //Slide offscreen to bottom
    GRect start = GRect(0, init_y, 140 , 60);
    GRect finish = GRect(0, 160, 140 , 60);
    animate_layer(text_layer_get_layer(comp_text_layer), &start, &finish, 300, 500);
  }
  //End animation on hour change
  else if((minute == 0) && (seconds == 0))
  {
    snprintf(text_buffer, sizeof(text_buffer), "%s", compliments[hour % 6]);

    //Change the TextLayer text to show the new time!
    text_layer_set_text(comp_text_layer, text_buffer);


    //Slide onscreen from the left
    GRect start = GRect(0, 160, 140 , 60);
    GRect finish = GRect(0, init_y, 140 , 60);
    animate_layer(text_layer_get_layer(comp_text_layer), &start, &finish, 300, 500);

    // Update the Clock_Layer
    snprintf(hourHex, sizeof(hourHex), "%d", hour);
    snprintf(minHex, sizeof(minHex), "%02d", minute);  
    snprintf(time_buffer, sizeof(time_buffer), "%s:%s", hourHex, minHex);
    text_layer_set_text(clock_text_layer, time_buffer);

  }
  //Redraw the time every minute
  else if (seconds == 0)
  {
    // Update the Clock_Layer
    snprintf(hourHex, sizeof(hourHex), "%d", hour);
    snprintf(minHex, sizeof(minHex), "%02d", minute);  
    snprintf(time_buffer, sizeof(time_buffer), "%s:%s", hourHex, minHex);
    text_layer_set_text(clock_text_layer, time_buffer);
  }
  //Set initial compliment and clock layers
  else if (first){
    snprintf(text_buffer, sizeof(text_buffer), "%s", compliments[hour % 6]);

    text_layer_set_text(comp_text_layer, text_buffer);
    
    // Update the Clock_Layer
    snprintf(hourHex, sizeof(hourHex), "%d", hour);
    snprintf(minHex, sizeof(minHex), "%02d", minute); 
    snprintf(time_buffer, sizeof(time_buffer), "%s:%s", hourHex, minHex);
    text_layer_set_text(clock_text_layer, time_buffer);
    first = false;
  }    
  
}

static void main_window_load(Window *window){

  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  GRect layer_bounds = GRect(0, 0, window_bounds.size.w - 40, 50);
  
  // Create output Additional_Text_Layer
  Layer *additional_text_layer_layer;
  additional_text_layer = text_layer_create(layer_bounds);
  text_layer_set_text_alignment(additional_text_layer, GTextAlignmentCenter);
  text_layer_set_font(additional_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(additional_text_layer, GColorBlack);
  text_layer_set_text(additional_text_layer, "It's");
  additional_text_layer_layer = text_layer_get_layer(additional_text_layer);
  layer_set_frame(additional_text_layer_layer, GRect(0, 10, window_bounds.size.w , 20));

  // Create output Clock_Text_Layer
  Layer *clock_text_layer_layer;
  clock_text_layer = text_layer_create(layer_bounds);
  text_layer_set_text_alignment(clock_text_layer, GTextAlignmentCenter);
  text_layer_set_font(clock_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_color(clock_text_layer, GColorBlack);
  text_layer_set_text(clock_text_layer, "");
  clock_text_layer_layer = text_layer_get_layer(clock_text_layer);
  layer_set_frame(clock_text_layer_layer, GRect(0, 30, window_bounds.size.w, 50));
  
  // Create output Write_Text_Layer
  Layer *write_text_layer_layer;
  write_text_layer = text_layer_create(layer_bounds);
  text_layer_set_text_alignment(write_text_layer, GTextAlignmentCenter);
  text_layer_set_font(write_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(write_text_layer, GColorBlack);
  text_layer_set_text(write_text_layer, "And you are still my");
  write_text_layer_layer = text_layer_get_layer(write_text_layer);
  layer_set_frame(write_text_layer_layer, GRect(0, 80, window_bounds.size.w , 35));
  
  // Create output Compliment_Text_Layer
  Layer *comp_text_layer_layer;
  comp_text_layer = text_layer_create(window_bounds);
  text_layer_set_text_alignment(comp_text_layer, GTextAlignmentCenter);
  script_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ACTION_MAN_30));
  text_layer_set_font(comp_text_layer, script_font);
  text_layer_set_text_color(comp_text_layer, GColorBlack);
  text_layer_set_text(comp_text_layer, "");
  comp_text_layer_layer = text_layer_get_layer(comp_text_layer);
  layer_set_frame(comp_text_layer_layer, GRect(0, init_y, 140 , 60));
  

  
  // Add text_layer to window_layer
  layer_add_child(window_layer, clock_text_layer_layer);
  layer_add_child(window_layer, write_text_layer_layer);
  layer_add_child(window_layer, additional_text_layer_layer);
  layer_add_child(window_layer, comp_text_layer_layer);
}

static void main_window_unload(Window *window){
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