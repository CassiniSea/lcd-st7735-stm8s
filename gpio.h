#ifndef __GPIO_ASM_H
#define __GPIO_ASM_H

// === PIN MASKS (0 bytes of Flash, calculated at compile time) ===
#define GPIO_PIN_0    ((uint8_t)0x01)
#define GPIO_PIN_1    ((uint8_t)0x02)
#define GPIO_PIN_2    ((uint8_t)0x04)
#define GPIO_PIN_3    ((uint8_t)0x08)
#define GPIO_PIN_4    ((uint8_t)0x10)
#define GPIO_PIN_5    ((uint8_t)0x20)
#define GPIO_PIN_6    ((uint8_t)0x40)
#define GPIO_PIN_7    ((uint8_t)0x80)

/* ============================================================================ */
/* MASTER MACROS FOR ONE-LINE DIRECT INITIALIZATION (Safe for neighboring pins) */
/* Uses atomic bitwise operations to prevent trashing other pins configuration. */
/* ============================================================================ */

/* Configures chosen pins as Output Push-Pull, High level, 10MHz Fast speed */
#define GPIO_INIT_OUTPUT(gpioPort, pinMask) \
	gpioInitOutput((void*)&(gpioPort)->ODR, (pinMask))
	
/* Configures chosen pins as Input with internal Pull-Up resistor enabled */
#define GPIO_INIT_INPUT_PULLUP(gpioPort, pinMask) \
	gpioInitInput((void*)&(gpioPort)->ODR, (pinMask))
	
/* Enables external interrupts on the chosen input pins (Pure MOV/OR inline) */
#define GPIO_IT_ENABLE(gpioPort, pinMask) \
do { \
    (gpioPort)->CR2 |= (pinMask); \
} while(0)

/* Disables external interrupts on the chosen input pins (Pure MOV/AND inline) */
#define GPIO_IT_DISABLE(gpioPort, pinMask) \
do { \
    (gpioPort)->CR2 &= (uint8_t)(~(pinMask)); \
} while(0)
	
/* Sets target pins to a High logic level (1). Compiles into atomic BSET. */
#define GPIO_WRITE_HIGH(gpioPort, pinMask) \
	do { \
		(gpioPort)->ODR |= (pinMask); \
	} while(0)

/* Drives target pins to a Low logic level (0). Compiles into atomic BRES. */
#define GPIO_WRITE_LOW(gpioPort, pinMask) \
	do { \
		(gpioPort)->ODR &= (uint8_t)(~(pinMask)); \
	} while(0)

/* Toggles (inverts) the logical state of target pins using XOR. */
#define GPIO_WRITE_REVERCE(gpioPort, pinMask) \
	do { \
		(gpioPort)->ODR ^= (pinMask); \
	} while(0)

/* Reads the logical state of a specific input pin. */
/* Returns 0x00 if pin is Low, or the pin's bitmask value if pin is High. */
#define GPIO_READ_INPUT_PIN(gpioPort, pinMask) \
	((uint8_t)((gpioPort)->IDR & (pinMask)))

// === LOW-LEVEL PROTOTYPES (Called by macros) ===
// gpioOdrAddr - (void*)&GPIOx->ODR
void gpioInitOutput(void* gpioOdrAddr, uint8_t pinMask);
void gpioInitInput(void* gpioOdrAddr, uint8_t pinMask);

#endif /* __GPIO_ASM_H */