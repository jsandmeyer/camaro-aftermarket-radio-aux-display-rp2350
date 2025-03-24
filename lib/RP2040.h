// "fixes" files like CAN2040 that include the wrong file
// includes the correct (wrapper) for RP2350 and adds a def for pio_sm_hw

#ifndef RP2040_H
#define RP2040_H

#include <RP2350Wrapper.h>

#ifndef pio_sm_hw
#define pio_sm_hw pio_sm_hw_t
#endif

#endif