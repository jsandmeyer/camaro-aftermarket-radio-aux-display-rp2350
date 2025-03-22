#include <Arduino.h>
#include "Debug.h"

void Debug::processDebugInput(const RendererContainer* renderers) {
#if DO_DEBUG == 1
    while (Serial.available()) {
        Serial.print("\n");
        switch (const auto input = Serial.read(); input) {
            case 'm':
            case 'i': {
                const uint8_t units = input == 'm'
                    ? GMLAN_VAL_CLUSTER_UNITS_METRIC
                    : GMLAN_VAL_CLUSTER_UNITS_IMPERIAL;
                for (Renderer *renderer : *renderers) {
                    renderer->setUnits(units);
                }
                break;
            }
            case 't': {
                uint8_t b[] = { 0, 0x72, 0, 0, 0, 0, 0, 0 };
                (*renderers)[1]->processMessage(0x212, b);
                break;
            }
            case 'p': {
                uint8_t b[8] = { GMLAN_VAL_PARK_ASSIST_ON, 0x33, 0x22, 0x00, 0, 0, 0, 0 };
                (*renderers)[0]->processMessage(0x1D4, b);
                break;
            }
            default:
                Serial.printf("Unrecognized input '%c'\n", input);
            break;
        }
    }
#endif
}
