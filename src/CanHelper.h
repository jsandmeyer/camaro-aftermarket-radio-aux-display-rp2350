#ifndef CANHELPER_H
#define CANHELPER_H

#include <can2040.h>

class CanHelper {
    CAN2040* canBus = nullptr;
public:
    void Handle(CAN2040* cd, CAN2040::NotificationType notify, CAN2040::Message* msg, uint32_t errorCode) volatile;
    void IRQ() volatile;
    explicit CanHelper(CAN2040* bus);
};

#endif //CANHELPER_H
