#include "Renderer.h"

/**
 * Sets new cluster units
 * @param newUnits the new unit data (GMLAN_VAL_CLUSTER_UNITS_*)
 */
void Renderer::setUnits(const uint8_t newUnits) {
    this->units = newUnits;

    if (canRender()) {
        needsRender = true;
    }
}

void Renderer::setDisplay(Adafruit_SSD1306 *display) {
    this->display = display;
}
