#include "network.h"
#include "windows/recording_window.h"

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
				recording_window_pop();
				break;
		}
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}

void network_init() {
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

void network_deinit() {

}

void start_network() {
	network_init();
}

void stop_network() {
	network_deinit();
}