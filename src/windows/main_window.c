#include "main_window.h"
#include "select_window.h"

static Window *s_main_window;
static TextLayer *s_start_record_layer;
static ActionBarLayer *s_action_start_record_layer;

static GBitmap *s_tick_bitmap;

static void record_click_handler(ClickRecognizerRef recognzier, Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Record Button Clicked");
	switch (click_recognizer_get_button_id(recognzier)) {
		case BUTTON_ID_SELECT:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Pushing Select Window");
		select_window_push();
			break;
		default:
			break;
	}
}

static void click_config_provider(void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Register Click Handler");
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)record_click_handler);
}

static void main_window_load(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Loaded");
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	const GEdgeInsets label_insets = {.top = (bounds.size.h / 2) - 24, .right = ACTION_BAR_WIDTH, .left = ACTION_BAR_WIDTH / 2};

	s_start_record_layer = text_layer_create(grect_inset(bounds, label_insets));
	text_layer_set_background_color(s_start_record_layer,GColorClear);
	text_layer_set_text(s_start_record_layer,"Start Recording");
	text_layer_set_text_alignment(s_start_record_layer,GTextAlignmentCenter);
	text_layer_set_font(s_start_record_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer,text_layer_get_layer(s_start_record_layer));

	s_tick_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TICK);

	s_action_start_record_layer = action_bar_layer_create();
	action_bar_layer_set_icon(s_action_start_record_layer,BUTTON_ID_SELECT,s_tick_bitmap);
	action_bar_layer_set_click_config_provider(s_action_start_record_layer,click_config_provider);
	action_bar_layer_add_to_window(s_action_start_record_layer,window);
}

static void main_window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Unloaded");
	action_bar_layer_destroy(s_action_start_record_layer);
	gbitmap_destroy(s_tick_bitmap);
	text_layer_destroy(s_start_record_layer);
	window_destroy(window);
	s_main_window = NULL;
}

void main_window_push() {
	if (!s_main_window)
	{
		s_main_window = window_create();
		window_set_background_color(s_main_window,PBL_IF_COLOR_ELSE(GColorChromeYellow, GColorWhite));
		window_set_window_handlers(s_main_window, (WindowHandlers) {
			.load = main_window_load,
			.unload = main_window_unload
		});
		window_stack_push(s_main_window,true);
	}
}