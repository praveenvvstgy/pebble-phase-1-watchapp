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

	accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);

	logging_session = data_logging_create(ACCEL_ID,DATA_LOGGING_BYTE_ARRAY,12,false);

	accel_data_service_subscribe(25,accel_data_handler);

}

static void logger_deinit() {
	data_logging_finish(logging_session);
	accel_data_service_unsubscribe();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Logging Session Complete");
}

void start_logger(int index) {
	s_index_of_event = index;
	logger_init();
}

void stop_logger() {
	logger_deinit();
}