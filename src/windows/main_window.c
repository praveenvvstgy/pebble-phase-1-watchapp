#include "main_window.h"
#include "recording_window.h"

static Window *s_main_window;
static TextLayer *s_start_record_layer;

typedef enum {
  AppKeySessionStatus = 0,  // Key: 0
  AppKeyActivityType		// Key: 1
} AppKeys;

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  // A new message has been successfully received
	Tuple *status_tuple = dict_find(iter, AppKeySessionStatus);
	if (status_tuple)
	{
		int32_t status = status_tuple->value->int32;
		switch (status) {
			case 1:
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Recording Start Command received from android");
				Tuple *activity_tuple = dict_find(iter, AppKeyActivityType);
				if (activity_tuple)
				{
					int32_t activity = activity_tuple->value->int32;
					recording_window_push((int)activity);
				}
				break;
			case 2:
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Recording Stop Command received from android");
				
				break;
		}
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}

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

	// Largest expected inbox and outbox message sizes
	const uint32_t inbox_size = 64;
	const uint32_t outbox_size = 64;

	// Open AppMessage
	app_message_open(inbox_size, outbox_size);

	// Register to be notified about inbox received events
	app_message_register_inbox_received(inbox_received_callback);

	// Register to be notified about inbox dropped events
	app_message_register_inbox_dropped(inbox_dropped_callback);
}

static void main_window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Unloaded");
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