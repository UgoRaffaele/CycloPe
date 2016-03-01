#include <pebble_worker.h>
#include "../src/storage.h"

static struct event current_page[PAGE_LENGTH];
static unsigned index;
static uint32_t num_samples = 2;

static void append_event(struct event *event) {
    
	if (index >= PAGE_LENGTH) {
		APP_LOG(APP_LOG_LEVEL_ERROR, "invalid value %u for index", index);
		return;
	}

	current_page[index] = *event;
	index = (index + 1) % PAGE_LENGTH;

	int ret = persist_write_data(1, current_page, sizeof current_page);
	if (ret < 0 || (unsigned)ret != sizeof current_page) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "unexpected return value %d for persist_wride_data", ret);
        return;
    }

}

static void new_event(AccelData data) {
    
	struct event event;
    event.x = data.x;
    event.y = data.y;
    event.z = data.z;
    event.did_vibrate = data.did_vibrate;
    event.timestamp = data.timestamp;
	append_event(&event);
    
}

static void data_handler(AccelData *data, uint32_t num_samples) {
    
    for (uint32_t i = 0; i < num_samples; i++) {
        if (data[i].timestamp > 0) {
            new_event(data[i]);
        }
    }
    
}

static bool init(void) {
    
	int ret = persist_read_data(1, current_page, sizeof current_page);
	if (ret == E_DOES_NOT_EXIST) {
		APP_LOG(APP_LOG_LEVEL_INFO, "no configuration found, initializing to zero");
		memset(current_page, 0, sizeof current_page);
	} else if (ret != sizeof current_page) {
		APP_LOG(APP_LOG_LEVEL_ERROR, "unexpected return value %d for persist_read_data", ret);
		return false;
	} else if (current_page[0].timestamp) {
		for (index = 1; index < PAGE_LENGTH && current_page[index - 1].timestamp < current_page[index].timestamp; index += 1);
	} else {
        index = 0;
    }

    accel_data_service_subscribe(num_samples, &data_handler);
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
	return true;
    
}

static void deinit(void) {
    
    accel_data_service_unsubscribe();
    
}

int main(void) {
    if (!init()) return 1;
    worker_event_loop();
    deinit();
    return 0;
}