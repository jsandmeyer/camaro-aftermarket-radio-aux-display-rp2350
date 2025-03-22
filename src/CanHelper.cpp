#include <RP2350.h>
#include <SerialUSB.h>
#include <hardware/irq.h>
#include <core_cm33.h>
#include <can2040.h>

#include "CanHelper.h"

#include <unordered_set>

#include "GMLan.h"

static volatile CanHelper *globalCanHelper = nullptr;

static void globalInvokeIRQHandler() {
    globalCanHelper->invokeIRQHandler();
}

CAN2040::ReceiveCallback CanHelper::getHandler() const {
    return [this](CAN2040* cd, const CAN2040::NotificationType notify, CAN2040::Message* msg, const uint32_t errorCode) {
        switch (notify) {
            case CAN2040::NOTIFY_RX:
                // ok
                break;
            case CAN2040::NOTIFY_TX:
                Serial.println("Sending CAN2040 message - should not have happened");
                return;
            case CAN2040::NOTIFY_ERROR:
                Serial.printf("Received CAN2040 error %lx\n", errorCode);
                return;
            default:
                Serial.println("Received CAN2040 unknown message - should not have happened");
                return;
        }

        auto const arbId = GMLAN_ARB(msg->id);

        if (arbId == GMLAN_MSG_CLUSTER_UNITS) {
            const uint8_t units = msg->data[0] & 0x0F;
            Serial.printf("New cluster units: 0x%02x\n", units);

            for (Renderer *renderer : *renderers) {
                renderer->setUnits(units);
            }

            return;
        }

        if (arbIds.find(arbId) == arbIds.end()) {
            return;
        }

        for (Renderer *renderer : *renderers) {
            Serial.printf("Processing via %s ARB ID 0x%08lx\n", renderer->getName(), arbId);
            renderer->processMessage(arbId, msg->data);
        }
    };
}

void CanHelper::invokeIRQHandler() const volatile {
    canBus->pioIrqHandler();
}

CanHelper::CanHelper(CAN2040* canBus, RendererContainer *renderers): canBus(canBus), renderers(renderers) {
    globalCanHelper = this;

    canBus->setup(GMLAN_CAN_PIO);
    canBus->callbackConfig(this->getHandler());

    irq_set_exclusive_handler(GMLAN_CAN_PIO_IRQn, globalInvokeIRQHandler);
    NVIC_SetPriority(GMLAN_CAN_PIO_IRQn, GMLAN_CAN_PRI);
    NVIC_EnableIRQ(GMLAN_CAN_PIO_IRQn);

    for (Renderer *renderer : *renderers) {
        for (auto arbId : renderer->getArbIds()) {
            arbIds.insert(arbId);
        }
    }
}

void CanHelper::start() const {
    canBus->start(GMLAN_CAN_BITRATE, GMLAN_CAN_RX, GMLAN_CAN_TX);
}
