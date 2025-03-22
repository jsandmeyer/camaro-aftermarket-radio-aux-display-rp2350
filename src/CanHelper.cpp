#include <RP2350.h>
#include <SerialUSB.h>
#include <hardware/irq.h>
#include <core_cm33.h>
#include <can2040.h>

#include "CanHelper.h"
#include "GMLan.h"

//static volatile CAN2040 *canBus = nullptr;
static volatile CanHelper *ch = nullptr;

static void invokeIRQHandler() {
    if (!ch) {
       // todo panic
    }
    ch->IRQ();
}

static void invokeHandle(CAN2040* cd, CAN2040::NotificationType notify, CAN2040::Message* msg, uint32_t errorCode) {
    if (!ch) {
       // todo panic
    }
    ch->Handle(cd, notify, msg, errorCode);
}

void CanHelper::Handle(CAN2040* cd, CAN2040::NotificationType notify, CAN2040::Message* msg, uint32_t errorCode) volatile {
    switch (notify) {
        case CAN2040::NOTIFY_RX:
            break;
        case CAN2040::NOTIFY_TX:
            Serial.println("Sending CAN2040 message");
        return;
        case CAN2040::NOTIFY_ERROR:
            Serial.printf("Received CAN2040 error %lx\n", errorCode);
        return;
        default:
            Serial.println("Received CAN2040 unknown");
        return;
    }

    auto canId = msg->id;
    auto buf = msg->data;
    auto const arbId = GMLAN_ARB(canId);

    if (arbId != GMLAN_MSG_TEMPERATURE && arbId != GMLAN_MSG_PARK_ASSIST && arbId != GMLAN_MSG_CLUSTER_UNITS) {
        return;
    }

    Serial.printf("Got: %lx -> %x %x %x %x %x %x %x %x\n", arbId, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    // todo pass back
}

void CanHelper::IRQ() volatile {
    canBus->pioIrqHandler();
}

CanHelper::CanHelper(CAN2040* bus): canBus(bus) {
    if (ch) {
       // todo panic
    }
    ch = this;

    // Setup canbus
    canBus->setup(0);
    canBus->callbackConfig(invokeHandle);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, invokeIRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    canBus->start(33333, 26, 27);
}
