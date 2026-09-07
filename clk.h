#ifndef __CLK_H
#define __CLK_H

// ============================================================================
// CLK_CKDIVR: CLOCK DIVIDER REGISTER MASKS
// F_master = F_hsi / HSIDIV, F_cpu = F_master / CPUDIV
// ============================================================================
#define CLK_PRESCALER_HSIDIV_1    ((uint8_t)0x00) /* HSI clock divided by 1 */
#define CLK_PRESCALER_CPUDIV_1    ((uint8_t)0x00) /* Master clock divided by 1 */

// ============================================================================
// CLK_PCKENR1: PERIPHERAL CLOCK GATING REGISTER 1 MASKS
// ============================================================================
#define CLK_PERIPH_TIM1           ((uint8_t)0x80)
#define CLK_PERIPH_TIM3           ((uint8_t)0x40) /* TIM4 for STM8S103 */
#define CLK_PERIPH_TIM2           ((uint8_t)0x20)
#define CLK_PERIPH_TIM4           ((uint8_t)0x10)
#define CLK_PERIPH_UART1          ((uint8_t)0x08)
#define CLK_PERIPH_SPI            ((uint8_t)0x02)
#define CLK_PERIPH_I2C            ((uint8_t)0x01)
#define CLK_PERIPH_ALL            ((uint8_t)0xFF)

// ============================================================================
// MASTER MACROS FOR CLOCK MANAGEMENT (0 bytes of Flash overhead)
// ============================================================================

/* Sets MCU frequency to 16MHz (HSI/1, CPU/1) using atomic MOV instruction */
#define CLK_INIT_16MHZ() \
do { \
    CLK->CKDIVR = (CLK_PRESCALER_HSIDIV_1 | CLK_PRESCALER_CPUDIV_1); \
} while(0)

/* Safely enables chosen peripherals clocking via logical OR operation */
#define CLK_PERIPH_ENABLE(periph_mask) \
do { \
    CLK->PCKENR1 |= (periph_mask); \
} while(0)

#endif /* __CLK_H */