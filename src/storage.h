#ifndef CYCLOPE_STORAGE_H
#define CYCLOPE_STORAGE_H

struct __attribute__((__packed__)) event {
    int16_t x;
    int16_t y;
    int16_t z;
    bool did_vibrate;
    uint64_t timestamp;
};

#define PAGE_LENGTH (PERSIST_DATA_MAX_LENGTH / sizeof(struct event))

#endif