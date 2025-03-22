// Must include RP2350Wrapper first, or else we get a ton of warnings
// Need to import files in correct order with correct undefine safety
#include <RP2350Wrapper.h>
#include <SerialUSB.h>
#include <can2040.h>

#include "CanHelper.h"
#include "RendererContainer.h"

// communications
constexpr auto SER_BAUD = 115200UL;

/**
 * Main entrypoint
 * Called by int main() by framework
 */
void setup() {
    sleep_ms(5000);
    Serial.begin(SER_BAUD);
    Serial.println("Booting up");

    delay(10);

    const auto renderers = new RendererContainer(2);

    const auto canBus = new CAN2040();
    const auto canHelper = new CanHelper(canBus, renderers);

    canHelper->start();

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
