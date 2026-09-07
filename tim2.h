
#ifndef __TIM2_ASM_H
#define __TIM2_ASM_H

// ============================================================================
// TIM2_CR1: CONTROL REGISTER 1 BIT MASKS
// ============================================================================
#define TIM2_CR1_ARPE_ENABLE      ((uint8_t)0x80) // 1: Auto-reload preload register is buffered
#define TIM2_CR1_ARPE_DISABLE     ((uint8_t)0x00) // 0: Auto-reload preload register is NOT buffered
#define TIM2_CR1_CEN_ENABLE       ((uint8_t)0x01) // 1: Counter enabled (Timer starts running)
#define TIM2_CR1_CEN_DISABLE      ((uint8_t)0x00) // 0: Counter disabled (Timer stops)
#define TIM2_CR1_CEN_ENABLE       ((uint8_t)0x01) // 1: Counter Enable

// ============================================================================
// TIM2_IER: INTERRUPT ENABLE REGISTER BIT MASKS
// ============================================================================
#define TIM2_IER_UIE_ENABLE       ((uint8_t)0x01) // 1: Update Interrupt Enabled (on overflow)
#define TIM2_IER_UIE_DISABLE      ((uint8_t)0x00) // 0: Update Interrupt Disabled

// ============================================================================
// TIM2_PSCR: PRESCALER VALUES (F_timer = F_cpu / 2^value)
// ============================================================================
#define TIM2_PRESCALER_1          ((uint8_t)0x00) // 2^0 = Clock divided by 1
#define TIM2_PRESCALER_2          ((uint8_t)0x01) // 2^1 = Clock divided by 2
#define TIM2_PRESCALER_4          ((uint8_t)0x02) // 2^2 = Clock divided by 4
#define TIM2_PRESCALER_8          ((uint8_t)0x03) // 2^3 = Clock divided by 8
#define TIM2_PRESCALER_16         ((uint8_t)0x04) // 2^4 = Clock divided by 16
#define TIM2_PRESCALER_32         ((uint8_t)0x05) // 2^5 = Clock divided by 32
#define TIM2_PRESCALER_64         ((uint8_t)0x06) // 2^6 = Clock divided by 64
#define TIM2_PRESCALER_128        ((uint8_t)0x07) // 2^7 = Clock divided by 128
#define TIM2_PRESCALER_256        ((uint8_t)0x08) // 2^8 = Clock divided by 256
#define TIM2_PRESCALER_512        ((uint8_t)0x09) // 2^9 = Clock divided by 512
#define TIM2_PRESCALER_1024       ((uint8_t)0x0A) // 2^10 = Clock divided by 1024
#define TIM2_PRESCALER_2048       ((uint8_t)0x0B) // 2^11 = Clock divided by 2048
#define TIM2_PRESCALER_4096       ((uint8_t)0x0C) // 2^12 = Clock divided by 4096
#define TIM2_PRESCALER_8192       ((uint8_t)0x0D) // 2^13 = Clock divided by 8192
#define TIM2_PRESCALER_16384      ((uint8_t)0x0E) // 2^14 = Clock divided by 16384

// ============================================================================
// TIM2_CCMRx: CAPTURE/COMPARE MODE REGISTERS BIT MASKS (For CH1, CH2, CH3)
// ============================================================================
// OCxPE: Output Compare Preload Enable
#define TIM2_CCMR_OCxPE_ENABLE    ((uint8_t)0x08) // 1: Preload register enabled for PWM duty cycles
#define TIM2_CCMR_OCxPE_DISABLE   ((uint8_t)0x00) // 0: Preload register disabled

// OCxM[2:0]: Output Compare Mode Options
#define TIM2_CCMR_OCxM_PWM1       ((uint8_t)0x60) // PWM Mode 1: Active when Counter < Compare
#define TIM2_CCMR_OCxM_PWM2       ((uint8_t)0x70) // PWM Mode 2: Active when Counter > Compare
#define TIM2_CCMR_OCxM_TIMING     ((uint8_t)0x00) // Frozen/Timing Mode: Used for pure timer tasks

// CCxS[1:0]: Capture/Compare Selection
#define TIM2_CCMR_CCxS_OUTPUT     ((uint8_t)0x00) // 00: Channel is configured as OUTPUT

// ============================================================================
// TIM2_CCERx: CAPTURE/COMPARE ENABLE REGISTERS BIT MASKS
// ============================================================================
// TIM2_CCER1 Masks (For Channel 1 and Channel 2)
#define TIM2_CCER1_CH1_ENABLE     ((uint8_t)0x01) // 1: Enable Channel 1 Output Pin
#define TIM2_CCER1_CH1_DISABLE    ((uint8_t)0x00) // 0: Disable Channel 1 Output Pin
#define TIM2_CCER1_CH1_POL_HIGH   ((uint8_t)0x00) // 0: Channel 1 Active High Polarity
#define TIM2_CCER1_CH1_POL_LOW    ((uint8_t)0x02) // 2: Channel 1 Active Low Polarity

#define TIM2_CCER1_CH2_ENABLE     ((uint8_t)0x10) // 1: Enable Channel 2 Output Pin
#define TIM2_CCER1_CH2_DISABLE    ((uint8_t)0x00) // 0: Disable Channel 2 Output Pin
#define TIM2_CCER1_CH2_POL_HIGH   ((uint8_t)0x00) // 0: Channel 2 Active High Polarity
#define TIM2_CCER1_CH2_POL_LOW    ((uint8_t)0x20) // 2: Channel 2 Active Low Polarity

// TIM2_CCER2 Masks (For Channel 3)
#define TIM2_CCER2_CH3_ENABLE     ((uint8_t)0x01) // 1: Enable Channel 3 Output Pin
#define TIM2_CCER2_CH3_DISABLE    ((uint8_t)0x00) // 0: Disable Channel 3 Output Pin
#define TIM2_CCER2_CH3_POL_HIGH   ((uint8_t)0x00) // 0: Channel 3 Active High Polarity
#define TIM2_CCER2_CH3_POL_LOW    ((uint8_t)0x02) // 2: Channel 3 Active Low Polarity

// === IER: INTERRUPT ENABLE REGISTERS ===
#define TIM2_INT_ENABLE           ((uint8_t)0x01) // Update Interrupt Enable
#define TIM2_INT_DISABLE          ((uint8_t)0x00)

// === CHANNEL CONFIGURATION MODES (0 bytes of Flash) ===
// Format: 16-bit number. 
// High byte = Value for CCMRx register. Low byte = Channel selection (1, 2, 3 or 0)
#define TIM2_PWM_OFF              ((uint16_t)0x0000) // Pure timer mode, all channels off
#define TIM2_PWM_CH1              ((uint16_t)(0x7801)) // PWM Mode 2 + Preload on Channel 1
#define TIM2_PWM_CH2              ((uint16_t)(0x7802)) // PWM Mode 2 + Preload on Channel 2
#define TIM2_PWM_CH3              ((uint16_t)(0x7803)) // PWM Mode 2 + Preload on Channel 3

// === UNIVERSAL MASTER MACRO FOR ONE-LINE INITIALIZATION ===
// Uses a clean C multi-line block expression. Compiles to pure direct MOV instructions.
#define TIM2_INIT(prescaler, period, pwmMode, interruptMode) \
do { \
    TIM2->PSCR = (prescaler); \
    TIM2->ARRH = (uint8_t)((uint16_t)(period) >> 8); \
    TIM2->ARRL = (uint8_t)((uint16_t)(period) & 0xFF); \
    TIM2->CR1  = TIM2_CR1_ARPE_ENABLE; \
    TIM2->IER  = (interruptMode); \
    /* Inline preprocessor compile-time routing for selected PWM channel */ \
    if ((pwmMode) == TIM2_PWM_CH1) { \
        TIM2->CCMR1 = (uint8_t)((pwmMode) >> 8); \
        TIM2->CCER1 = 0x01; /* Enable CH1 output, active High */ \
    } else if ((pwmMode) == TIM2_PWM_CH2) { \
        TIM2->CCMR2 = (uint8_t)((pwmMode) >> 8); \
        TIM2->CCER1 = 0x10; /* Enable CH2 output, active High */ \
    } else if ((pwmMode) == TIM2_PWM_CH3) { \
        TIM2->CCMR3 = (uint8_t)((pwmMode) >> 8); \
        TIM2->CCER2 = 0x01; /* Enable CH3 output inside CCER2 */ \
    } \
} while(0)

/* Starts TIM2 counter using atomic BSET instruction */
#define TIM2_ENABLE() \
do { \
    TIM2->CR1 |= TIM2_CR1_CEN_ENABLE; \
} while(0)

/* Stops TIM2 counter using atomic BRES instruction */
#define TIM2_DIASBLE() \
do { \
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_CEN_ENABLE); \
} while(0)

// ============================================================================
// NEW ULTRA-FAST RUNTIME MACROS (0 bytes of function cost)
// Hardware constraint: Always write CCRxH FIRST, then CCRxL for coherence!
// ============================================================================
#define TIM2_SET_COMPARE1(compareVal) \
do { \
    TIM2->CCR1H = (uint8_t)((uint16_t)(compareVal) >> 8); \
    TIM2->CCR1L = (uint8_t)((uint16_t)(compareVal) & 0xFF); \
} while(0)

#define TIM2_SET_COMPARE2(compareVal) \
do { \
    TIM2->CCR2H = (uint8_t)((uint16_t)(compareVal) >> 8); \
    TIM2->CCR2L = (uint8_t)((uint16_t)(compareVal) & 0xFF); \
} while(0)

#define TIM2_SET_COMPARE3(compareVal) \
do { \
    TIM2->CCR3H = (uint8_t)((uint16_t)(compareVal) >> 8); \
    TIM2->CCR3L = (uint8_t)((uint16_t)(compareVal) & 0xFF); \
} while(0)

// ============ TIM2 PACKED CONFIGURATION MASKS (0 bytes of Flash) ============
// Upper 4 bits = Channel Selector (0=None, 1=CH1, 2=CH2, 3=CH3)
// Lower 4 bits = Feature Selector (0=Pure, 1=Int Only, 2=PWM Only, 3=PWM+Int)
// ============================================================================
#define TIM2_MODE_PURE_TIMER      ((uint8_t)0x00)
#define TIM2_MODE_INT_ONLY        ((uint8_t)0x01)

#define TIM2_MODE_CH1_PWM         ((uint8_t)0x12)
#define TIM2_MODE_CH1_PWM_INT     ((uint8_t)0x13)

#define TIM2_MODE_CH2_PWM         ((uint8_t)0x22)
#define TIM2_MODE_CH2_PWM_INT     ((uint8_t)0x23)

#define TIM2_MODE_CH3_PWM         ((uint8_t)0x32)
#define TIM2_MODE_CH3_PWM_INT     ((uint8_t)0x33)

#endif /* __TIM2_ASM_H */