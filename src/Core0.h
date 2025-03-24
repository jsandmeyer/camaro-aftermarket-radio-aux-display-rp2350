#ifndef CORE0_H
#define CORE0_H

#include <can2040.h>
#include <pico/util/queue.h>
#include <unordered_set>

#include "GMLAN.h"

class Core0 {
    queue_t* messageQueue;
    std::unordered_set<uint32_t> arbIds = {GMLAN_MSG_CLUSTER_UNITS, GMLAN_MSG_PARK_ASSIST, GMLAN_MSG_TEMPERATURE};
    void canBusMessageCallback(CAN2040* cd, CAN2040::NotificationType notify, CAN2040::Message* msg, uint32_t errorCode);
public:
    explicit Core0(queue_t* messageQueue);
};

#endif //CORE0_H
