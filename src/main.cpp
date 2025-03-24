#include <RP2350Wrapper.h> // must include first to avoid warnings
#include <SerialUSB.h>
#include <can2040.h>
#include <pico/sem.h>
#include <pico/util/queue.h>

#include "Debug.h"
#include "Flash.h"

#include "Core0.h"
#include "Core1.h"

constexpr auto SER_BAUD = 115200UL;

static queue_t messageQueue;
static semaphore_t setupSemaphore;

static Core0* core0 = nullptr;
static Core1* core1 = nullptr;

void setup1() {
    delay(100);
    DEBUG(Serial.print("Starting core1\n"));
    sem_acquire_blocking(&setupSemaphore);
    DEBUG(Serial.print("Core1 init semaphore acquired\n"));

    core1 = new Core1(&messageQueue);
}

void loop1() {
    core1->processMessage();
    core1->renderDisplay();
}

void setup() {
    // don't wait, init semaphore now
    sem_init(&setupSemaphore, 0, 1);

    DEBUG(Serial.begin(SER_BAUD));
    DEBUG(Serial.print("Starting core0\n"));

    Flash::setDefaults();

    queue_init(&messageQueue, sizeof(CAN2040::Message), 128);
    sem_release(&setupSemaphore);
    DEBUG(Serial.print("Core0 init semaphore released\n"));

    core0 = new Core0(&messageQueue);
}

void loop() {
    NO_DEBUG(tight_loop_contents());
    Debug::processDebugInput(&messageQueue);
}
