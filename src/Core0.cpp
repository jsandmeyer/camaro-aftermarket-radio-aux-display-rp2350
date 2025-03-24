#include "Core0.h"

#include <RP2350Wrapper.h> // must include first to avoid warnings
#include <SerialUSB.h>

#include "Debug.h"
#include "Flash.h"

#include "Core1.h"

static auto canBus = new CAN2040();

void canBusIRQHandler() {
    canBus->pioIrqHandler();
}

void Core0::canBusMessageCallback(CAN2040* cd, const CAN2040::NotificationType notify, CAN2040::Message* msg, const uint32_t errorCode) {
    if (notify == CAN2040::NOTIFY_ERROR) {
        DEBUG(Serial.printf("Received CAN2040 error %lx\n", errorCode));
        return;
    }

    if (notify != CAN2040::NOTIFY_RX) {
        DEBUG(Serial.printf("Received CAN2040 unexpected message - should not have happened %lx", notify));
        return;
    }

    const auto arbId = GMLAN_ARB(msg->id);

    if (arbIds.find(arbId) == arbIds.end()) {
        // DEBUG(Serial.printf("Wrong 0x%08lx -> 0x%08lx\n", msg->id, GMLAN_ARB(msg->id)));
        return;
    }

    if (arbId == GMLAN_MSG_CLUSTER_UNITS) {
        Flash::saveUnits(msg->data[0] & 0x0FU);
    }

    if (queue_is_full(messageQueue)) {
        DEBUG(Serial.print("WARNING QUEUE WAS FULL\n"));
    } else {
        queue_try_add(messageQueue, msg);
    }
}

Core0::Core0(queue_t* messageQueue): messageQueue(messageQueue) {
    canBus->setup(GMLAN_CAN_PIO);

    canBus->callbackConfig([this](auto && PH1, auto && PH2, auto && PH3, auto && PH4) {
        canBusMessageCallback(
            std::forward<decltype(PH1)>(PH1),
            std::forward<decltype(PH2)>(PH2),
            std::forward<decltype(PH3)>(PH3),
            std::forward<decltype(PH4)>(PH4)
        );
    });

    irq_set_exclusive_handler(GMLAN_CAN_PIO_IRQn, canBusIRQHandler);
    NVIC_SetPriority(GMLAN_CAN_PIO_IRQn, GMLAN_CAN_PRI);
    NVIC_EnableIRQ(GMLAN_CAN_PIO_IRQn);

    canBus->start(GMLAN_CAN_BITRATE, GMLAN_CAN_RX, GMLAN_CAN_TX);

    DEBUG(Serial.print("Ready core0\n"));
}
