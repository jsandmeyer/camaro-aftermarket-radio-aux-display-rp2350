#include <Arduino.h>
#include "Debug.h"
#include "GMLan.h"

void Debug::tryEnqueue(queue_t* messageQueue, CAN2040::Message* message) {
    Serial.printf(
        "DEBUG MESSAGE ARB ID=0x%08lx BUF={0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x}\n",
        message->id,
        message->data[0],
        message->data[1],
        message->data[2],
        message->data[3],
        message->data[4],
        message->data[5],
        message->data[6],
        message->data[7]
    );

    if (queue_is_full(messageQueue)) {
        DEBUG(Serial.println("WARNING QUEUE WAS FULL"));
    } else {
        queue_try_add(messageQueue, message);
    }

    free(message);
}

void Debug::processDebugInput(queue_t* messageQueue) {
#if DO_DEBUG == 1
    while (Serial.available()) {
        Serial.print("\n");
        switch (const auto input = Serial.read(); input) {
            case 'm':
                tryEnqueue(messageQueue, new CAN2040::Message({
                    .id = GMLAN_R_ARB(GMLAN_MSG_CLUSTER_UNITS),
                    .dlc = 0,
                    .data = { GMLAN_VAL_CLUSTER_UNITS_METRIC, 0, 0, 0, 0, 0, 0, 0 },
                }));
                break;
            case 'i': {
                tryEnqueue(messageQueue, new CAN2040::Message({
                    .id = GMLAN_R_ARB(GMLAN_MSG_CLUSTER_UNITS),
                    .dlc = 0,
                    .data = { GMLAN_VAL_CLUSTER_UNITS_IMPERIAL, 0, 0, 0, 0, 0, 0, 0 },
                }));
                break;
            }
            case 't': {
                tryEnqueue(messageQueue, new CAN2040::Message({
                    .id = GMLAN_R_ARB(GMLAN_MSG_TEMPERATURE),
                    .dlc = 0,
                    .data = { 0, 0x72, 0, 0, 0, 0, 0, 0 },
                }));
                break;
            }
            case 'p': {
                tryEnqueue(messageQueue, new CAN2040::Message({
                    .id = GMLAN_R_ARB(GMLAN_MSG_PARK_ASSIST),
                    .dlc = 0,
                    .data = { GMLAN_VAL_PARK_ASSIST_ON, 0x33, 0x22, 0x00, 0, 0, 0, 0 },
                }));
                break;
            }
            case 'q': {
                tryEnqueue(messageQueue, new CAN2040::Message({
                    .id = GMLAN_R_ARB(GMLAN_MSG_PARK_ASSIST),
                    .dlc = 0,
                    .data = { GMLAN_VAL_PARK_ASSIST_OFF, 0, 0, 0, 0, 0, 0, 0 },
                }));
                break;
            }
            default:
                Serial.printf("Unrecognized input '%c'\n", input);
            break;
        }
    }
#endif
}
