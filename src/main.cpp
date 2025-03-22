// Must include RP2350Wrapper first, or else we get a ton of warnings
// Need to import files in correct order with correct undefine safety
#include <RP2350Wrapper.h>
#include <SerialUSB.h>
#include <can2040.h>

#include "Debug.h"
#include "OLED.h"
#include "Renderer.h"
#include "RendererContainer.h"
#include "CanHelper.h"
#include "GMParkAssist.h"
#include "GMTemperature.h"

// communications
constexpr auto SER_BAUD = 115200UL;

/**
 * Render data to display
 * @param display
 * @param renderers
 * @param lastRenderer
 */
void renderDisplay(Adafruit_SSD1306* display, const RendererContainer* renderers, Renderer*& lastRenderer) {
    /*
     * Render new data, based on priority, taking the first which "should render"
     * It is always assumed that if a module "should render" that it has new data and must render now
     */
    for (Renderer *renderer : *renderers) {
        if (renderer->shouldRender()) {
            DEBUG(Serial.printf("Rendering [1] via %s\n", renderer->getName()));
            renderer->render();
            lastRenderer = renderer;
            return; // exit loop
        }
    }

    /*
     * Render old data, based on priority, taking the first which "can render"
     * Only the first module which "can render" is considered, this avoids oscillation in display choice
     */
    for (Renderer *renderer : *renderers) {
        if (renderer->canRender()) {
            // if we just rendered, don't waste time re-rendering
            if (lastRenderer == renderer) {
                return;
            }

            DEBUG(Serial.printf("Rendering [2] via %s\n", renderer->getName()));
            renderer->render();
            lastRenderer = renderer;
            return; // exit loop
        }
    }

    /*
     * If there is absolutely nothing that should or can be rendered, clear the display
     */
    display->clearDisplay();
    display->display();
}

/**
 * Main entrypoint
 * Called by int main() by framework
 */
void setup() {
    sleep_ms(5000);
    DEBUG(Serial.begin(SER_BAUD));
    DEBUG(Serial.println("Booting up"));

    delay(10);

    SPI1.setRX(OLED_RX);
    SPI1.setCS(OLED_CS);
    SPI1.setSCK(OLED_SCK);
    SPI1.setTX(OLED_TX);

    const auto display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI1, OLED_DC, OLED_RST, SPI_CS_PIN_OLED, OLED_SPI_BAUD);

    while (!display->begin(SSD1306_SWITCHCAPVCC)) {
        DEBUG(Serial.println("Error configuring SSD1306"));
        delay(500);
    }

    display->clearDisplay();
    display->display();

    const auto renderers = new RendererContainer(2);

    Renderer *lastRenderer = nullptr; // last renderer to render, to avoid doubles of same data
    // renderers->setRenderer(0, new GMParkAssist(display));
    renderers->setRenderer(1, new GMTemperature(display));

    const auto canBus = new CAN2040();
    const auto canHelper = new CanHelper(canBus, renderers);

    canHelper->start();

    // ReSharper disable once CppDFAEndlessLoop
    while (true) {
        renderDisplay(display, renderers, lastRenderer);
        Debug::processDebugInput(renderers);
    }
}

/**
 * Loop entrypoint
 * Called by int main() from framework, but realistically will never run because setup() is [[noreturn]]
 */
void loop() {
    /* do nothing - loop handled inside setup() */
}