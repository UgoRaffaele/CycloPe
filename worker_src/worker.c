#include <pebble_worker.h>

#define ACCEL_BATCH 1
#define ACCEL_XYZ_THRESHOLD 1000

DataLoggingSessionRef accel_data_log;

#define SQRT_MAGIC_F 0x5f3759df 
float sqrt_(const float x) {
  const float xhalf = 0.5f*x;
  union {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = SQRT_MAGIC_F - (u.i >> 1);  // gives initial guess y0
  u.x = u.x*(1.5f - xhalf*u.x*u.x);   // This can be removed for increasing speed
  u.x = u.x*(1.5f - xhalf*u.x*u.x);   // This can be removed for increasing speed
  return x*u.x*(1.5f - xhalf*u.x*u.x);// Newton step, repeating increases accuracy 
}

int64_t sqrt__(int64_t val){
	return (int64_t)sqrt_((float)val);
}

static void data_handler(AccelData *data, uint32_t num_samples) {
    
    if (num_samples >= ACCEL_BATCH) {
        
        int64_t accel_xyz = sqrt__((data->x * data->x) + (data->y * data->y) + (data->z * data->z)) - 1000; 
        APP_LOG(APP_LOG_LEVEL_INFO, "accel_xyz = %ld", (long)accel_xyz);
        
        if (accel_xyz >= ACCEL_XYZ_THRESHOLD) {
            
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