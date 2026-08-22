#ifndef __CLK_ASM_H
#define __CLK_ASM_H

// === ћј— » ƒЋя CLK_PCKENR1 (0 байт пам€ти, вычисл€ютс€ при компил€ции) ===
#define CLK_PERIPH_TIM1   (1 << 7)
#define CLK_PERIPH_TIM2   (1 << 5)
#define CLK_PERIPH_TIM3   (1 << 6) // (ƒл€ STM8S103 это TIM4, в зависимости от заголовочника)
#define CLK_PERIPH_TIM4   (1 << 4)
#define CLK_PERIPH_UART1  (1 << 3)
#define CLK_PERIPH_SPI    (1 << 1)
#define CLK_PERIPH_I2C    (1 << 0)

void clkInit(void);
void clkPeripheralEnable(uint8_t mask);

#endif /* __CLK_ASM_H */