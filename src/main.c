#include <pebble.h>

Window *window;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static Layer *s_radar_layer;

static void render_radar(Layer* layer, GContext* ctx) {
  //graphics_context_set_stroke_color(ctx, GColorJaegerGreen);
#if PBL_COLOR
  graphics_context_set_fill_color(ctx, GColorJaegerGreen);
#else
  graphics_context_set_fill_color(ctx, GColorWhite);
#endif // PBL_COLOR
  //graphics_draw_circle(ctx, GPoint(72, 60), 10);
  graphics_fill_circle(ctx, GPoint(72, 60), 10);
} 

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
  
  // Set the background
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_background);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Draw the pulse circle
  s_radar_layer = layer_create(GRect(0, 0, 144, 168));
  //bitmap_layer_set_background_color(radarLayer, GColorClear);
  layer_set_update_proc(s_radar_layer, (LayerUpdateProc)render_radar);
  layer_add_child(window_get_root_layer(window), s_radar_layer);

	// Push the window
	window_stack_push(window, true);
	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}

void handle_deinit(void) {
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  layer_destroy(s_radar_layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}