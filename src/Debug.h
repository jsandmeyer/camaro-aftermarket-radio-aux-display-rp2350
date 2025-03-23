#ifndef DEBUG_H
#define DEBUG_H

#include <pico/util/queue.h>

#include "Flash.h"

#if DO_DEBUG == 1
    #define DEBUG(X) X
    #define NO_DEBUG(X)
#else
    #define DEBUG(X)
    #define NO_DEBUG(X) X
#endif

#include <can2040.h>

class Debug {
    static void tryEnqueue(queue_t* messageQueue, CAN2040::Message* message);
public:
    static void processDebugInput(queue_t* messageQueue, Flash* flash);
};


#endif //DEBUG_H
