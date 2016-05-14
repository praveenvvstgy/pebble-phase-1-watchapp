#include "recording_window.h"
#include "modules/logger.h"

static Window *s_recording_window;
static TextLayer *s_recording_indicator_layer;
static TextLayer *s_recording_time_elapsed_layer;
static AppTimer *s_timer;

static int s_minutes_elapsed;

static int s_selected_event_index;

static void record_click_handler(ClickRecognizerRef recognzier, Window *window) {
	switch (click_recognizer_get_button_id(recognzier)) {
		case BUTTON_ID_BACK:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Popping Record Screen since Back button was clicked");
			recording_window_pop();
		default:
			break;
	}
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler)record_click_handler);
}

static void update_elapsed_time(void *data) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "One more minute elapsed..");
	s_minutes_elapsed++;
	static char s_buffer[30];
	snprintf(s_buffer, sizeof(s_buffer), "Time Elapsed: %2d minute(s)", s_minutes_elapsed);
	text_layer_set_text(s_recording_time_elapsed_layer, s_buffer);
	s_timer = app_timer_register(1000*60,update_elapsed_time,NULL);
}

static void recording_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	s_recording_indicator_layer = text_layer_create(grect_inset(bounds, GEdgeInsets((bounds.size.h / 4) - 28,0,bounds.size.h / 2,0)));
	text_layer_set_text(s_recording_indicator_layer,"Recording...");
	text_layer_set_font(s_recording_indicator_layer,fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_recording_indicator_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_recording_indicator_layer,GColorClear);
	layer_add_child(window_layer,text_layer_get_layer(s_recording_indicator_layer));

	s_recording_time_elapsed_layer = text_layer_create(grect_inset(bounds, GEdgeInsets(bounds.size.h / 2,0,(bounds.size.h / 4) - 24,0)));
	text_layer_set_text(s_recording_time_elapsed_layer,"Time Elapsed: 0 minute(s)");
	text_layer_set_font(s_recording_time_elapsed_layer,fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(s_recording_time_elapsed_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_recording_time_elapsed_layer,GColorClear);
	layer_add_child(window_layer,text_layer_get_layer(s_recording_time_elapsed_layer));

	s_minutes_elapsed = 0;
	s_timer = app_timer_register(1000*60,update_elapsed_time,NULL);

	start_logger(s_selected_event_index);
}

static void recording_window_unload(Window *window) {
	stop_logger();
	text_layer_destroy(s_recording_indicator_layer);
	text_layer_destroy(s_recording_time_elapsed_layer);
	app_timer_cancel(s_timer);
	window_destroy(window);
	s_recording_window = NULL;
}

void recording_window_push(int index) {
	if (!s_recording_window)
	{
		s_selected_event_index = index;
		s_recording_window = window_create();
		window_set_background_color(s_recording_window,GColorLightGray);
		window_set_window_handlers(s_recording_window,(WindowHandlers) {
			.load = recording_window_load,
			.unload = recording_window_unload
		});
		window_stack_push(s_recording_window,true);
	}
}

void recording_window_pop() {
	window_stack_remove(s_recording_window,true);
}