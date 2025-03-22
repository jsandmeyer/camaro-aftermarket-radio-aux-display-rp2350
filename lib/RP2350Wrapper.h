#include <RP2350.h>

// SPI1 from RP2350 messes up framework lib SPI.h declaration of SPI1 class
// let the framework win because we need it and not the RP2350 value

#ifdef SPI1
#undef SPI1
#endif
