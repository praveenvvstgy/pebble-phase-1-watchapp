#include "main_window.h"
#include "recording_window.h"
#include "modules/network.h"

static Window *s_main_window;
static TextLayer *s_start_record_layer;

static void main_window_load(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Loaded");
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	const GEdgeInsets label_insets = {.top = (bounds.size.h / 2) - 24};

	s_start_record_layer = text_layer_create(grect_inset(bounds, label_insets));
	text_layer_set_background_color(s_start_record_layer,GColorClear);
	text_layer_set_text(s_start_record_layer,"Open Android to Start Recording");
	text_layer_set_text_alignment(s_start_record_layer,GTextAlignmentCenter);
	text_layer_set_font(s_start_record_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer,text_layer_get_layer(s_start_record_layer));

	start_network();
}

static void main_window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Unloaded");
	stop_network();
	text_layer_destroy(s_start_record_layer);
	window_destroy(window);
	s_main_window = NULL;
}

void main_window_push() {
	if (!s_main_window)
	{
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating Main Window");
		s_main_window = window_create();
		window_set_background_color(s_main_window,PBL_IF_COLOR_ELSE(GColorChromeYellow, GColorWhite));
		window_set_window_handlers(s_main_window, (WindowHandlers) {
			.load = main_window_load,
			.unload = main_window_unload
		});
		window_stack_push(s_main_window,true);
	}
}