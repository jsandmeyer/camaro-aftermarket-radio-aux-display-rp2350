// Ignore warnings for modules, seems RP2350 sdk has redefinition problems
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <RP2350.h>
#include <SerialUSB.h>
#include <hardware/irq.h>
#include <core_cm33.h>
#include <can2040.h>
#pragma GCC diagnostic pop

#include "GMLan.h"

// communications
constexpr auto SER_BAUD = 115200UL;

CAN2040* canBus;

void my_cb(CAN2040* cd, CAN2040::NotificationType notify, CAN2040::Message* msg, uint32_t errorCode) {
    switch (notify) {
        case CAN2040::NOTIFY_RX:
        break;
        case CAN2040::NOTIFY_TX:
            Serial.println("Sending CAN2040 message");
        return;
        case CAN2040::NOTIFY_ERROR:
            Serial.printf("Received CAN2040 error %lx\n", errorCode);
        return;
        default:
            Serial.println("Received CAN2040 unknown");
        return;
    }

    auto canId = msg->id;
    auto buf = msg->data;
    auto const arbId = GMLAN_ARB(canId);

    if (arbId != GMLAN_MSG_TEMPERATURE && arbId != GMLAN_MSG_PARK_ASSIST && arbId != GMLAN_MSG_CLUSTER_UNITS) {
        return;
    }

    Serial.printf("Got: %lx -> %x %x %x %x %x %x %x %x\n", arbId, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
}


static void PIOx_IRQHandler() {
    canBus->pioIrqHandler();
}

void canbus_setup() {
    uint32_t pio_num = 0;
    uint32_t bitrate = 33333; // was 500K
    uint32_t gpio_rx = 26, gpio_tx = 27; // was 4,5

    // Setup canbus
    canBus->setup(pio_num);
    canBus->callbackConfig(my_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    canBus->start(bitrate, gpio_rx, gpio_tx);
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

    canBus = new CAN2040();
    canbus_setup();
    // initializeCanBus(canBus, watchdog);

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
