#ifndef RENDERER_H
#define RENDERER_H

#include <RP2350Wrapper.h>
#include <Adafruit_SSD1306.h>

#include "GMLan.h"

class Renderer {
protected:
    /**
     * Whether this module needs to be rendered.
     */
    bool needsRender = false;

    /**
     * Current unit choice
     */
    uint8_t units = GMLAN_VAL_CLUSTER_UNITS_METRIC;

    /**
     * OLED display
     */
    Adafruit_SSD1306 *display = nullptr;
public:
    virtual ~Renderer() = default;

    /**
     * Create a Renderer
     * @param display OLED display
     */
    explicit Renderer(Adafruit_SSD1306 *display): display(display) {}

    /**
     * Process a GMLAN message
     * @param arbId the Arbitration ID
     * @param buf the buffer data
     */
    virtual void processMessage(uint32_t arbId, uint8_t buf[8]);

    /**
     * Renders data to the display
     */
    virtual void render();

    /**
     * Determine whether there is an update which should be shown on the display now
     * Should return true if there is new data, or if this module needs to make sure its data is shown
     * @return whether the module should render
     */
    virtual bool shouldRender();

    /**
     * Determine whether there is data which could be shown on the display
     * Should return true if there is any low-priority data
     * @return whether the module can render
     */
    virtual bool canRender();

    /**
     * Returns the name of this renderer
     * @return the name as a string
     */
    [[nodiscard]] virtual const char* getName() const;

    /**
     * Sets new cluster units
     * @param newUnits the new unit data (GMLAN_VAL_CLUSTER_UNITS_*)
     */
    void setUnits(uint8_t newUnits);

    /**
     * Update the SSD1306 object
     * @param display the new SSD1306 object
     */
    void setDisplay(Adafruit_SSD1306 *display);
};

#endif //RENDERER_H
