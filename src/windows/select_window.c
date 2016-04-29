#include "select_window.h"
#include "recording_window.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 4

char kactivities[NUM_FIRST_MENU_ITEMS][25] = {"Sitting", "Walking", "Running", "Jumping"};

static Window *s_select_window;
static SimpleMenuLayer *s_activity_select_menu;
static SimpleMenuSection s_activity_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_activity_menu_items[NUM_FIRST_MENU_ITEMS];

static void activity_menu_select_callback(int index, void *ctx) {
	window_stack_pop(true);
	recording_window_push(index);
}

void select_window_load(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Select Window Loaded");
	Layer *window_layer = window_get_root_layer(window);
	// GRect bounds = layer_get_bounds(window_layer);
	GRect frame = layer_get_frame(window_layer);

	s_activity_menu_sections[0] = (SimpleMenuSection) {
		.num_items = NUM_FIRST_MENU_ITEMS,
		.items = s_activity_menu_items
	};

	for(int num_a_items = 0; num_a_items < NUM_FIRST_MENU_ITEMS; num_a_items++) {
		s_activity_menu_items[num_a_items] = (SimpleMenuItem) {
			.title = kactivities[num_a_items],
			.callback = activity_menu_select_callback,
		};
	}

	s_activity_select_menu = simple_menu_layer_create(frame,window,s_activity_menu_sections,NUM_MENU_SECTIONS,NULL);
	layer_add_child(window_layer,simple_menu_layer_get_layer(s_activity_select_menu));
}

void select_window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Select Window unloaded");
	simple_menu_layer_destroy(s_activity_select_menu);
	window_destroy(window);
	s_select_window = NULL;
}

void select_window_push() {
	if (!s_select_window)
	{
		s_select_window = window_create();
		window_set_background_color(s_select_window,PBL_IF_COLOR_ELSE(GColorVividCerulean, GColorWhite));
		window_set_window_handlers(s_select_window,(WindowHandlers) {
			.load = select_window_load,
			.unload = select_window_unload
		});
		window_stack_push(s_select_window,true);
	}
}