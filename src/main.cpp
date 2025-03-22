// Must include RP2350Wrapper first, or else we get a ton of warnings
// Need to import files in correct order with correct undefine safety
#include <RP2350Wrapper.h>
#include <SerialUSB.h>
#include <can2040.h>
#include <pico/util/queue.h>

#include "Debug.h"
#include "OLED.h"
#include "Renderer.h"
#include "GMLan.h"
#include "GMParkAssist.h"
#include "GMTemperature.h"

constexpr auto SER_BAUD = 115200UL;

static queue_t messageQueue;
static auto canBus = new CAN2040();
static std::unordered_set arbIds = {GMLAN_MSG_CLUSTER_UNITS, GMLAN_MSG_PARK_ASSIST, GMLAN_MSG_TEMPERATURE};

void canBusIRQHandler() {
    canBus->pioIrqHandler();
}

void canBusMessageCallback(CAN2040* cd, const CAN2040::NotificationType notify, CAN2040::Message* msg, const uint32_t errorCode) {
    if (notify == CAN2040::NOTIFY_ERROR) {
        Serial.printf("Received CAN2040 error %lx\n", errorCode);
        return;
    }

    if (notify != CAN2040::NOTIFY_RX) {
        Serial.printf("Received CAN2040 unexpected message - should not have happened %lx", notify);
        return;
    }

    if (arbIds.find(GMLAN_ARB(msg->id)) == arbIds.end()) {
        return;
    }

    if (queue_is_full(&messageQueue)) {
        DEBUG(Serial.println("WARNING QUEUE WAS FULL"));
    } else {
        queue_try_add(&messageQueue, msg);
    }
}

void processMessage(std::vector<Renderer *> renderers) {
    CAN2040::Message msg;
    queue_try_remove(&messageQueue, &msg);

    if (msg.id == 0) {
        DEBUG(Serial.println("WARNING MSG IS NULL"));
        return;
    }

    auto const arbId = GMLAN_ARB(msg.id);

    if (arbId == GMLAN_MSG_CLUSTER_UNITS) {
        const uint8_t units = msg.data[0] & 0x0F;
        Serial.printf("New cluster units: 0x%02x\n", units);

        for (Renderer *renderer : renderers) {
            renderer->setUnits(units);
        }

        return;
    }

    for (Renderer *renderer : renderers) {
        Serial.printf("Checking %s : ARB ID 0x%08lx\n", renderer->getName(), arbId);
        renderer->processMessage(arbId, msg.data);
    }
}

/**
 * Render data to display
 * @param display
 * @param renderers
 * @param lastRenderer
 */
void renderDisplay(Adafruit_SSD1306* display, std::vector<Renderer *> renderers, Renderer*& lastRenderer) {
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

[[noreturn]] void core1Entry() {
    DEBUG(Serial.println("Starting core1"));
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

    const std::vector<Renderer *> renderers = {
        new GMParkAssist(display),
        new GMTemperature(display),
    };

    Renderer *lastRenderer = nullptr; // last renderer to render, to avoid doubles of same data

    while (true) {
        if (!queue_is_empty(&messageQueue)) {
            processMessage(renderers);
        }

        renderDisplay(display, renderers, lastRenderer);
    }
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

    queue_init(&messageQueue, sizeof(CAN2040::Message), 128);

    multicore_launch_core1(core1Entry);

    canBus->setup(GMLAN_CAN_PIO);
    canBus->callbackConfig(canBusMessageCallback);

    irq_set_exclusive_handler(GMLAN_CAN_PIO_IRQn, canBusIRQHandler);
    NVIC_SetPriority(GMLAN_CAN_PIO_IRQn, GMLAN_CAN_PRI);
    NVIC_EnableIRQ(GMLAN_CAN_PIO_IRQn);
}

/**
 * Loop entrypoint
 * Called by int main() from framework, but realistically will never run because setup() is [[noreturn]]
 */
void loop() {
    Debug::processDebugInput(&messageQueue);
    NO_DEBUG(tight_loop_contents());
}