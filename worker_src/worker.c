#include <pebble_worker.h>

#define ACCEL_BATCH 1

DataLoggingSessionRef accel_data_log;

static void data_handler(AccelData *data, uint32_t num_samples) {
    
    if (num_samples >= ACCEL_BATCH) {
        
        DataLoggingResult result = data_logging_log(accel_data_log, data, 1);
    
        if (result == DATA_LOGGING_BUSY) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "someone else is writing to this logging session");
        } else if (result == DATA_LOGGING_FULL) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "no more space to save data");
        } else if (result == DATA_LOGGING_NOT_FOUND) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "the logging session does not exist");
        } else if (result == DATA_LOGGING_CLOSED) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "The logging session was made inactive");
        } else if (result == DATA_LOGGING_INVALID_PARAMS) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "an invalid parameter was passed to one of the functions");
        }
        
    }
    
}

static bool init(void) {
    
    accel_data_service_subscribe(ACCEL_BATCH, &data_handler);
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
    
    accel_data_log = data_logging_create(
        0x1010,
        DATA_LOGGING_BYTE_ARRAY,
        sizeof(AccelData) * ACCEL_BATCH,
        false
    );
    
    return true;
    
}

static void deinit(void) {
    
    data_logging_finish(accel_data_log);
    accel_data_service_unsubscribe();
    
}

int main(void) {
    if (!init()) return 1;
    worker_event_loop();
    deinit();
    return 0;
}