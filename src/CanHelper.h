#ifndef CAN_HELPER_H
#define CAN_HELPER_H

#include <can2040.h>
#include "RendererContainer.h"

class CanHelper {
    CAN2040* canBus;
    RendererContainer* renderers;
    std::unordered_set<uint32_t> arbIds;
    [[nodiscard]] CAN2040::ReceiveCallback getHandler() const;
public:
    void invokeIRQHandler() const volatile;
    void start() const;
    CanHelper(CAN2040* canBus, RendererContainer *renderers);
};

#endif //CAN_HELPER_H
