#ifndef RP2350_WRAPPER_H
#define RP2350_WRAPPER_H

// include this first so duplicates can be #undef
#include <hardware/structs/pio.h>

// #undef things that overlap with RP2350 but for which there are no safety checks
#ifndef RP2350_H

#ifdef ACCESSCTRL_BASE
#undef ACCESSCTRL_BASE
#endif

#ifdef ADC_BASE
#undef ADC_BASE
#endif

#ifdef BOOTRAM_BASE
#undef BOOTRAM_BASE
#endif

#ifdef BUSCTRL_BASE
#undef BUSCTRL_BASE
#endif

#ifdef CLOCKS_BASE
#undef CLOCKS_BASE
#endif

#ifdef CORESIGHT_TRACE_BASE
#undef CORESIGHT_TRACE_BASE
#endif

#ifdef DMA_BASE
#undef DMA_BASE
#endif

#ifdef EPPB_BASE
#undef EPPB_BASE
#endif

#ifdef GLITCH_DETECTOR_BASE
#undef GLITCH_DETECTOR_BASE
#endif

#ifdef HSTX_CTRL_BASE
#undef HSTX_CTRL_BASE
#endif

#ifdef HSTX_FIFO_BASE
#undef HSTX_FIFO_BASE
#endif

#ifdef I2C0_BASE
#undef I2C0_BASE
#endif

#ifdef I2C1_BASE
#undef I2C1_BASE
#endif

#ifdef IO_BANK0_BASE
#undef IO_BANK0_BASE
#endif

#ifdef IO_QSPI_BASE
#undef IO_QSPI_BASE
#endif

#ifdef OTP_BASE
#undef OTP_BASE
#endif

#ifdef OTP_DATA_BASE
#undef OTP_DATA_BASE
#endif

#ifdef OTP_DATA_RAW_BASE
#undef OTP_DATA_RAW_BASE
#endif

#ifdef PADS_BANK0_BASE
#undef PADS_BANK0_BASE
#endif

#ifdef PADS_QSPI_BASE
#undef PADS_QSPI_BASE
#endif

#ifdef PIO0_BASE
#undef PIO0_BASE
#endif

#ifdef PIO1_BASE
#undef PIO1_BASE
#endif

#ifdef PIO2_BASE
#undef PIO2_BASE
#endif

#ifdef PLL
#undef PLL
#endif

#ifdef PLL_SYS_BASE
#undef PLL_SYS_BASE
#endif

#ifdef PLL_USB_BASE
#undef PLL_USB_BASE
#endif

#ifdef POWMAN_BASE
#undef POWMAN_BASE
#endif

#ifdef PPB_BASE
#undef PPB_BASE
#endif

#ifdef PSM_BASE
#undef PSM_BASE
#endif

#ifdef PWM_BASE
#undef PWM_BASE
#endif

#ifdef RESETS_BASE
#undef RESETS_BASE
#endif

#ifdef ROSC_BASE
#undef ROSC_BASE
#endif

#ifdef SHA256_BASE
#undef SHA256_BASE
#endif

#ifdef SIO_BASE
#undef SIO_BASE
#endif

#ifdef SPI0_BASE
#undef SPI0_BASE
#endif

#ifdef SPI1_BASE
#undef SPI1_BASE
#endif

#ifdef SYSCFG_BASE
#undef SYSCFG_BASE
#endif

#ifdef SYSINFO_BASE
#undef SYSINFO_BASE
#endif

#ifdef TBMAN_BASE
#undef TBMAN_BASE
#endif

#ifdef TICKS_BASE
#undef TICKS_BASE
#endif

#ifdef TIMER0_BASE
#undef TIMER0_BASE
#endif

#ifdef TIMER1_BASE
#undef TIMER1_BASE
#endif

#ifdef TRNG_BASE
#undef TRNG_BASE
#endif

#ifdef UART0_BASE
#undef UART0_BASE
#endif

#ifdef UART1_BASE
#undef UART1_BASE
#endif

#ifdef WATCHDOG_BASE
#undef WATCHDOG_BASE
#endif

#ifdef XIP_AUX_BASE
#undef XIP_AUX_BASE
#endif

#ifdef XIP_CTRL_BASE
#undef XIP_CTRL_BASE
#endif

#ifdef XOSC_BASE
#undef XOSC_BASE
#endif

#endif // RP2350_H

#include <RP2350.h>

// SPI1 from RP2350 messes up framework lib SPI.h declaration of SPI1 class
// let the framework win because we need it and not the RP2350 value

#ifdef SPI1
#undef SPI1
#endif

#endif
