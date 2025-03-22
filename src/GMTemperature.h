#ifndef GM_TEMPERATURE_H
#define GM_TEMPERATURE_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Renderer.h"

class GMTemperature final : public Renderer {
    /**
     * Most recently recorded temperature
     * Stored is 2 * (temperature in degrees Celsius + 40)
     */
    uint8_t temperature = 0;
public:
    /**
     * Process GMLAN message
     * @param arbId the arbitration ID GMLAN_MSG_TEMPERATURE
     * @param buf buffer data from GMLAN
     */
    void processMessage(uint32_t arbId, uint8_t buf[8]) override;

    /**
     * Renders the current Temperature display
     * Should only be called if there is something to render
     * Updates the display
     */
    void render() override;

    /**
     * Determines whether there is new data to render
     * Rendering should happen if the temperature changed
     * @return whether rendering should occur
     */
    bool shouldRender() override;

    /**
     * Determines whether there is data which can be rendered
     * Rendering may happen if there is temperature data
     * @return whether rendering can occur
     */
    bool canRender() override;

    /**
     * Returns the name of this renderer
     * @return the name as a string
     */
    [[nodiscard]] const char* getName() const override;
};



#endif //GM_TEMPERATURE_H
