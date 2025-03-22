// Ignore warnings for modules, seems RP2350 sdk has redefinition problems
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <RP2350.h>
#include <SerialUSB.h>
#include <hardware/irq.h>
#include <core_cm33.h>
#include <can2040.h>
#pragma GCC diagnostic pop

#include "CanHelper.h"

// communications
constexpr auto SER_BAUD = 115200UL;

void my_cb(CAN2040* cd, CAN2040::NotificationType notify, CAN2040::Message* msg, uint32_t errorCode) {
}


static void PIOx_IRQHandler() {
    canBus->pioIrqHandler();
}

void initializeCanBus(CAN2040* canBus) {
    // Setup canbus
    canBus->setup(0);
    canBus->callbackConfig(my_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    canBus->start(33333, 26, 27);
}

/**
 * Main entrypoint
 * Called by int main() by framework
 */
void setup() {
    sleep_ms(5000);
    Serial.begin(SER_BAUD);
    Serial.println("Booting up");

    delay(10);

    const auto canBus = new CAN2040();
    new CanHelper(canBus);

    pinMode(25, OUTPUT);
}

/**
 * Loop entrypoint
 * Called by int main() from framework, but realistically will never run because setup() is [[noreturn]]
 */
void loop() {
    /* do nothing - loop handled inside setup() */
    digitalWrite(25, HIGH);
    sleep_ms(500);
    digitalWrite(25, LOW);
    sleep_ms(500);
}
