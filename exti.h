#ifndef __EXTI_H
#define __EXTI_H

/* ============================================================================ */
/* EXTI: EXTERNAL INTERRUPT CONFIGURATION MASKS (0 bytes of Flash)              */
/* ============================================================================ */
#define EXTI_SET_SENSITIVITY(extiPortMasks) \
do { \
    EXTI->CR1 = (extiPortMasks); \
} while(0)

#define EXTI_SET_SENSITIVITY_E(extiPortMask) \
do { \
    EXTI->CR2 = (extiPortMask); \
} while(0)

/* --- Port Sensitivity Modes (2 bits per port) --- */
#define EXTI_A_FALL_LOW           ((uint8_t)(0x00 << 0)) 
#define EXTI_A_RISE_ONLY          ((uint8_t)(0x01 << 0)) 
#define EXTI_A_FALL_ONLY          ((uint8_t)(0x02 << 0)) 
#define EXTI_A_RISE_FALL          ((uint8_t)(0x03 << 0)) 

#define EXTI_B_FALL_LOW           ((uint8_t)(0x00 << 2))
#define EXTI_B_RISE_ONLY          ((uint8_t)(0x01 << 2))
#define EXTI_B_FALL_ONLY          ((uint8_t)(0x02 << 2))
#define EXTI_B_RISE_FALL          ((uint8_t)(0x03 << 2))

#define EXTI_C_FALL_LOW           ((uint8_t)(0x00 << 4))
#define EXTI_C_RISE_ONLY          ((uint8_t)(0x01 << 4))
#define EXTI_C_FALL_ONLY          ((uint8_t)(0x02 << 4))
#define EXTI_C_RISE_FALL          ((uint8_t)(0x03 << 4))

#define EXTI_D_FALL_LOW           ((uint8_t)(0x00 << 6))
#define EXTI_D_RISE_ONLY          ((uint8_t)(0x01 << 6))
#define EXTI_D_FALL_ONLY          ((uint8_t)(0x02 << 6))
#define EXTI_D_RISE_FALL          ((uint8_t)(0x03 << 6))

#define EXTI_E_FALL_LOW           ((uint8_t)(0x00 << 0))
#define EXTI_E_RISE_ONLY          ((uint8_t)(0x01 << 0))
#define EXTI_E_FALL_ONLY          ((uint8_t)(0x02 << 0))
#define EXTI_E_RISE_FALL          ((uint8_t)(0x03 << 0))

#endif /* __EXTI_H */