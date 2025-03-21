#include <Arduino.h>
// #include <RP2350.h>
#include <ACAN2040.h>

#include "Watchdog.h"

// communications
constexpr auto SER_BAUD = 115200UL;

ACAN2040* canBus;

void my_cb(can2040 *cd, uint32_t notify, can2040_msg *msg);

/**
 * Main entrypoint
 * Called by int main() by framework
 */
void setup() {
    Serial.begin(SER_BAUD);
    Serial.println("Booting up");

    delay(10);

    canBus = new ACAN2040(0, 27, 26, 33333, 150000000L, my_cb);
    // initializeCanBus(canBus, watchdog);

    pinMode(25, OUTPUT);
}

void my_cb(struct can2040 * cd, uint32_t notify, struct can2040_msg * msg) {
    switch (notify) {
        case CAN2040_NOTIFY_RX:
            Serial.println("Received CAN2040 message");
            break;
        case CAN2040_NOTIFY_TX:
            Serial.println("Sending CAN2040 message");
            return;
        case CAN2040_NOTIFY_ERROR:
            Serial.println("Received CAN2040 error");
            return;
        default:
            Serial.println("Received CAN2040 unknown");
            return;
    }

    auto canId = msg->id;
    auto buf = msg->data;

    Serial.printf("Got: %lx -> %x %x %x %x %x %x %x %x\n", canId, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
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
