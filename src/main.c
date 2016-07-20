#include <pebble.h>


#define MINOR_TICK_LENGTH 3
#define MINOR_TICK_WIDTH 1

#define MAJOR_TICK_LENGTH 5
#define MAJOR_TICK_WIDTH 3

// pointer to main window
static Window *s_main_window;

// pointer to main window layer
static Layer *s_main_window_layer;

// pointer to tick mark layer
static Layer *s_tick_mark_layer;


// courtesy of @robisodd
int32_t abs32(int32_t a) {return (a^(a>>31)) - (a>>31);}     // returns absolute value of A (only works on 32bit signed)

// courtesy of @robisodd
GPoint getPointOnRect(GRect r, int angle) {
  int32_t sin = sin_lookup(angle), cos = cos_lookup(angle);  // Calculate once and store, to make quicker and cleaner
  int32_t dy = sin>0 ? (r.size.h/2) : (0-r.size.h)/2;        // Distance to top or bottom edge (from center)
  int32_t dx = cos>0 ? (r.size.w/2) : (0-r.size.w)/2;        // Distance to left or right edge (from center)
  if(abs32(dx*sin) < abs32(dy*cos)) {                        // if (distance to vertical line) < (distance to horizontal line)
    dy = (dx * sin) / cos;                                   // calculate distance to vertical line
  } else {                                                   // else: (distance to top or bottom edge) < (distance to left or right edge)
    dx = (dy * cos) / sin;                                   // move to top or bottom line
  }
  return GPoint(dx+r.origin.x+(r.size.w/2), dy+r.origin.y+(r.size.h/2));  // Return point on rectangle
}


// Convenience function to draw a line in the graphics context with a given color between two points at a specified width
static void draw_line(GContext *ctx, GColor color, GPoint p1, GPoint p2, int width){
  
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, width);
  graphics_draw_line(ctx, p1, p2);
  
}




static void tick_mark_update_proc(Layer *this_layer, GContext *ctx) {
 
  for (int i=0; i<60; i++){
    
    #ifdef PBL_RECT
    if (i%5 == 0 ){
      draw_line(ctx, GColorWhite, getPointOnRect(layer_get_bounds(this_layer), TRIG_MAX_ANGLE * i / 60), getPointOnRect(grect_inset(layer_get_bounds(this_layer), GEdgeInsets(MAJOR_TICK_LENGTH)), TRIG_MAX_ANGLE * i / 60), MAJOR_TICK_WIDTH);
    }
    else
    {
      draw_line(ctx, GColorWhite, getPointOnRect(layer_get_bounds(this_layer), TRIG_MAX_ANGLE * i / 60), getPointOnRect(grect_inset(layer_get_bounds(this_layer), GEdgeInsets(MINOR_TICK_LENGTH)), TRIG_MAX_ANGLE * i / 60), MINOR_TICK_WIDTH);
    }
    #else
    if (i%5 == 0 ){
      draw_line(ctx, GColorWhite, gpoint_from_polar(layer_get_bounds(this_layer), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * i / 60), gpoint_from_polar(grect_inset(layer_get_bounds(this_layer), GEdgeInsets(MAJOR_TICK_LENGTH)), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * i / 60), MAJOR_TICK_WIDTH);
    }
    else
    {
      draw_line(ctx, GColorWhite, gpoint_from_polar(layer_get_bounds(this_layer), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * i / 60), gpoint_from_polar(grect_inset(layer_get_bounds(this_layer), GEdgeInsets(MINOR_TICK_LENGTH)), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * i / 60), MINOR_TICK_WIDTH);
    }
    #endif
        
  }
  
}




static void main_window_load(Window *window) {
  
  // get the main window layer
  s_main_window_layer = window_get_root_layer(s_main_window);
  
  // Get the boundaries of the main layer
  GRect s_main_window_bounds = layer_get_bounds(s_main_window_layer);
  
  // Create the layer we will draw on
  s_tick_mark_layer = layer_create(s_main_window_bounds);
  
  // Add the layer to our main window layer
  layer_add_child(s_main_window_layer, s_tick_mark_layer);

  // Set the update procedure for our layer
  layer_set_update_proc(s_tick_mark_layer, tick_mark_update_proc);
   
  // Tell the watchface layer it needs to redraw
  layer_mark_dirty(s_main_window_layer);
    
  
}


static void main_window_unload(Window *window) {
    
  // Destroy the main window
  window_destroy(s_main_window);
  
}


static void init(void) {
    
  // Create the main window
  s_main_window = window_create();
  
  // set the background colour
  window_set_background_color(s_main_window, GColorBlack);
  
  // set the window load and unload handlers
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  // show the window on screen
  window_stack_push(s_main_window, true);
  
}


static void deinit(void) {
  
  // Destroy the main window
  window_destroy(s_main_window);
  
}


int main(void) {
  
  init();
  app_event_loop();
  deinit();
  
}