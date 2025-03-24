#include <RP2350Wrapper.h> // must include first to avoid warnings
#include <SerialUSB.h>
#include <can2040.h>
#include <pico/util/queue.h>

#include "util/Debug.h"
#include "util/Flash.h"

#include "Core0.h"
#include "Core1.h"

constexpr auto SER_BAUD = 115200UL;
bool core1_separate_stack = true;

static queue_t messageQueue;

static Core0* core0 = nullptr;
static Core1* core1 = nullptr;

void setup() {
    DEBUG(Serial.begin(SER_BAUD));
    DEBUG(Serial.print("Starting core0\n"));

    Flash::setDefaults();

    queue_init(&messageQueue, sizeof(CAN2040::Message), 128);

    rp2040.fifo.push_nb(0xFFFF);
    DEBUG(Serial.print("Core0 init fifo sent\n"));

    core0 = new Core0(&messageQueue);
}

void loop() {
    NO_DEBUG(tight_loop_contents());
    Debug::processDebugInput(&messageQueue);
}

void setup1() {
    DEBUG(Serial.print("Starting core1\n"));

    // wait for ready message
    while (rp2040.fifo.pop() != 0xFFFF) {
        tight_loop_contents();
    }

    DEBUG(Serial.print("Core1 init fifo acquired\n"));

    core1 = new Core1(&messageQueue);
}

void loop1() {
    core1->processMessage();
    core1->renderDisplay();
}
