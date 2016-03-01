#include <pebble_worker.h>
#include "worker.h"
#include "../src/storage.h"

int main(void) {
    worker_event_loop();
    return 0;
}