#include "log_dest_rtt.h"

#include <common/freertos_mutex.hpp>
#include <task.h>
#include "SEGGER_RTT.h"

// Note: This is not required to be in CCMRAM and can be moved to regular RAM if needed.
static __attribute__((section(".ccmram"))) freertos::Mutex rtt_mutex;

static void rtt_put_char(char character, [[maybe_unused]] void *arg) {
    SEGGER_RTT_PutCharSkipNoLock(0, character);
}

static inline void initialize_rtt_subsystem() {
    static bool subsystem_initialized = false;
    if (subsystem_initialized == false) {
        SEGGER_RTT_Init();
        subsystem_initialized = true;
    }
}

static void rtt_log_event_unlocked(log_destination_t *destination, log_event_t *event) {
    destination->log_format_fn(event, rtt_put_char, NULL);
    rtt_put_char('\r', NULL);
    rtt_put_char('\n', NULL);
}

void rtt_log_event(log_destination_t *destination, log_event_t *event) {
    initialize_rtt_subsystem();

    // Do not use std::unique_lock here because #include <mutex> pulls in
    // a bunch of std::crap which breaks XL debug build due to FLASH inflation.
    rtt_mutex.lock();
    rtt_log_event_unlocked(destination, event);
    rtt_mutex.unlock();
}
