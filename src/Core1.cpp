#include <SerialUSB.h>

#include "Core1.h"
#include "renderers/GMParkAssist.h"
#include "renderers/GMTemperature.h"
#include "util/Debug.h"
#include "util/Flash.h"
#include "util/GMLan.h"
#include "util/OLED.h"

Core1::Core1(queue_t* messageQueue): messageQueue(messageQueue) {
    SPI1.setRX(OLED_RX);
    SPI1.setCS(OLED_CS);
    SPI1.setSCK(OLED_SCK);
    SPI1.setTX(OLED_TX);

    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI1, OLED_DC, OLED_RST, SPI_CS_PIN_OLED, OLED_SPI_BAUD);

    while (!display->begin(SSD1306_SWITCHCAPVCC)) {
        DEBUG(Serial.print("Error configuring SSD1306\n"));
        delay(500);
    }

    display->clearDisplay();
    display->display();

    renderers.push_back(new GMParkAssist(display));
    renderers.push_back(new GMTemperature(display));

    const auto units = Flash::getUnits();
    DEBUG(Serial.printf("Got units: %x\n", units));

    for (Renderer *renderer : renderers) {
        renderer->setUnits(units);
    }

    DEBUG(Serial.print("Ready core1\n"));
}

void Core1::processMessage() {
    CAN2040::Message msg{.id = 0};

    if (!queue_try_remove(messageQueue, &msg)) {
        return;
    }

    auto const arbId = GMLAN_ARB(msg.id);

    if (arbId == GMLAN_MSG_CLUSTER_UNITS) {
        const uint8_t units = msg.data[0] & 0x0FU;
        DEBUG(Serial.printf("New cluster units: 0x%02x\n", units));

        for (Renderer *renderer : renderers) {
            renderer->setUnits(units);
        }

        return;
    }

    for (Renderer *renderer : renderers) {
        DEBUG(Serial.printf("Checking %s : ARB ID 0x%08lx\n", renderer->getName(), arbId));
        renderer->processMessage(arbId, msg.data);
    }
}

void Core1::renderDisplay() {
    /*
     * Render new data, based on priority, taking the first which "should render"
     * It is always assumed that if a module "should render" that it has new data and must render now
     */
    for (Renderer *renderer : renderers) {
        if (renderer->shouldRender()) {
            DEBUG(if (renderer != lastRenderer) Serial.printf("Rendering [1] via %s\n", renderer->getName()));
            renderer->render();
            lastRenderer = renderer;
            return; // exit loop
        }
    }

    /*
     * Render old data, based on priority, taking the first which "can render"
     * Only the first module which "can render" is considered, this avoids oscillation in display choice
     */
    for (Renderer *renderer : renderers) {
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
