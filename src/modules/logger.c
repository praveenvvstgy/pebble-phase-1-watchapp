#include "logger.h"
#include <inttypes.h>

#define ACCEL_ID 1

typedef enum {
  AppKeySessionStatus = 0,  // Key: 0
  AppKeyActivityType		// Key: 1
} AppKeys;

// The session reference variable
static DataLoggingSessionRef logging_session;

static int s_index_of_event;

static int s_sample_count = 0;

static void send_int_message_from_outbox(int type, int value) {
	// Declare the dictionary's iterator
	DictionaryIterator *out_iter;
	// Prepare the outbox buffer for this message
	AppMessageResult result = app_message_outbox_begin(&out_iter);
	if(result == APP_MSG_OK) {
	  dict_write_int(out_iter, type, &value, sizeof(int), true);

	  // Send this message
	  result = app_message_outbox_send();
	  if(result != APP_MSG_OK) {
	    APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
	  }
	} else {
	  // The outbox cannot be used right now
	  APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
	}
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  // A new message has been successfully received

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}

static void outbox_sent_callback(DictionaryIterator *iter, void *context) {
  // The message just sent has been successfully delivered
	Tuple *status_tuple = dict_find(iter, AppKeySessionStatus);
	if (status_tuple)
	{
		int32_t status = status_tuple->value->int32;
		switch (status) {
			case 1:
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Recording Start Message ACK");
				send_int_message_from_outbox(AppKeyActivityType, s_index_of_event);
				break;
			case 2:
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Recording Stop Message ACK");
				break;
		}
	}

}

static void outbox_failed_callback(DictionaryIterator *iter,
                                      AppMessageResult reason, void *context) {
  // The message just sent failed to be delivered
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message send failed. Reason: %d", (int)reason);
}

static void encode_bytes(unsigned char destination[], uint8_t startidx, int64_t source, uint8_t length) {
  // Big endian encoding
  for (int i = 0; i < length; i++) {
    destination[startidx + i] = source >> 8 * (length - i - 1);
  }
}

static void accel_data_handler(AccelData *data, uint32_t num_samples) {
	unsigned char accel_data[num_samples][12];
	for (uint32_t i = 0; i < num_samples; ++i)
	{
		encode_bytes(accel_data[i], 0, data[i].x, 2);
		encode_bytes(accel_data[i], 2, data[i].y, 2);
    	encode_bytes(accel_data[i], 4, data[i].z, 2);
    	encode_bytes(accel_data[i], 6, data[i].timestamp, 6);
    	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d, %d, %d, %" PRId64 , data[i].x, data[i].y, data[i].z, data[i].timestamp);
	}
	data_logging_log(logging_session,&accel_data,num_samples);
	s_sample_count += num_samples;
}

static void logger_init() {
	// Largest expected inbox and outbox message sizes
	const uint32_t inbox_size = 64;
	const uint32_t outbox_size = 64;

	// Open AppMessage
	app_message_open(inbox_size, outbox_size);

	// Register to be notified about inbox received events
	app_message_register_inbox_received(inbox_received_callback);

	// Register to be notified about inbox dropped events
	app_message_register_inbox_dropped(inbox_dropped_callback);

	// Register to be notified about outbox sent events
	app_message_register_outbox_sent(outbox_sent_callback);

	// Register to be notified about outbox failed events
	app_message_register_outbox_failed(outbox_failed_callback);

	send_int_message_from_outbox(AppKeySessionStatus, 1);

	accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);

	logging_session = data_logging_create(ACCEL_ID,DATA_LOGGING_BYTE_ARRAY,12,false);

	accel_data_service_subscribe(25,accel_data_handler);

}

static void logger_deinit() {
	data_logging_finish(logging_session);
	accel_data_service_unsubscribe();
	send_int_message_from_outbox(AppKeySessionStatus, 2);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Logging Session Complete");
}

void start_logger(int index) {
	s_index_of_event = index;
	logger_init();
}

void stop_logger() {
	logger_deinit();
}