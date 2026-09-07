#ifndef __ITC_ASM_H
#define __ITC_ASM_H

/* ============================================================================ */
/* MASTER MACRO FOR ONE-LINE DIRECT MOV INITIALIZATION                          */
/* Takes 0xFF, safely clears required bits and injects new ones via compile-time*/
/* preprocessor execution. Generates exactly ONE 'mov' instruction (4 bytes).  */
/* Example: ITC_SET_PRIORITY(ITC->ISPR3, ISPR3_V11_TIM1_OVF_L1); */
/* ============================================================================ */
#define ITC_SET_PRIORITY(isprRegister, vectorPriorities) \
do { \
    (isprRegister) = (uint8_t)(vectorPriorities); \
} while(0)

/* ============================================================================ */
/* COMPILE-TIME BIT ROUTING MASKS                                               */
/* Base state is 0xFF.                                                          */
/* Level 1 (Med)  = bits 01 -> Force clear bit 1 of the pair                    */
/* Level 2 (High) = bits 00 -> Force clear both bits of the pair                */
/* Level 3 (Low)  = bits 11 -> Keeps default 0xFF state                         */
/* ============================================================================ */

/* --- REGISTER ISPR1 (Vectors 0 to 3) --- */
#define ISPR1_V0_TLI_L1           ((uint8_t)(0xFF & ~(0x02 << 0))) /* Force bits 01 (Med)  */
#define ISPR1_V0_TLI_L2           ((uint8_t)(0xFF & ~(0x03 << 0))) /* Force bits 00 (High) */
#define ISPR1_V0_TLI_L3           ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR1_V1_AWU_L1           ((uint8_t)(0xFF & ~(0x02 << 2))) /* Force bits 01 (Med)  */
#define ISPR1_V1_AWU_L2           ((uint8_t)(0xFF & ~(0x03 << 2))) /* Force bits 00 (High) */
#define ISPR1_V1_AWU_L3           ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR1_V2_CLK_L1           ((uint8_t)(0xFF & ~(0x02 << 4))) /* Force bits 01 (Med)  */
#define ISPR1_V2_CLK_L2           ((uint8_t)(0xFF & ~(0x03 << 4))) /* Force bits 00 (High) */
#define ISPR1_V2_CLK_L3           ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR1_V3_PORTA_L1         ((uint8_t)(0xFF & ~(0x02 << 6))) /* Force bits 01 (Med)  */
#define ISPR1_V3_PORTA_L2         ((uint8_t)(0xFF & ~(0x03 << 6))) /* Force bits 00 (High) */
#define ISPR1_V3_PORTA_L3         ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

/* --- REGISTER ISPR2 (Vectors 4 to 7) --- */
#define ISPR2_V4_PORTB_L1         ((uint8_t)(0xFF & ~(0x02 << 0))) /* Force bits 01 (Med)  */
#define ISPR2_V4_PORTB_L2         ((uint8_t)(0xFF & ~(0x03 << 0))) /* Force bits 00 (High) */
#define ISPR2_V4_PORTB_L3         ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR2_V5_PORTC_L1         ((uint8_t)(0xFF & ~(0x02 << 2))) /* Force bits 01 (Med)  */
#define ISPR2_V5_PORTC_L2         ((uint8_t)(0xFF & ~(0x03 << 2))) /* Force bits 00 (High) */
#define ISPR2_V5_PORTC_L3         ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR2_V6_PORTD_L1         ((uint8_t)(0xFF & ~(0x02 << 4))) /* Force bits 01 (Med)  */
#define ISPR2_V6_PORTD_L2         ((uint8_t)(0xFF & ~(0x03 << 4))) /* Force bits 00 (High) */
#define ISPR2_V6_PORTD_L3         ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR2_V7_PORTE_L1         ((uint8_t)(0xFF & ~(0x02 << 6))) /* Force bits 01 (Med)  */
#define ISPR2_V7_PORTE_L2         ((uint8_t)(0xFF & ~(0x03 << 6))) /* Force bits 00 (High) */
#define ISPR2_V7_PORTE_L3         ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

/* --- REGISTER ISPR3 (Vectors 8 to 11) --- */
#define ISPR3_V10_SPI_L1          ((uint8_t)(0xFF & ~(0x02 << 4))) /* Force bits 01 (Med)  */
#define ISPR3_V10_SPI_L2          ((uint8_t)(0xFF & ~(0x03 << 4))) /* Force bits 00 (High) */
#define ISPR3_V10_SPI_L3          ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR3_V11_TIM1_OVF_L1     ((uint8_t)(0xFF & ~(0x02 << 6))) /* Force bits 01 (Med)  -> Compiles to 0x7F */
#define ISPR3_V11_TIM1_OVF_L2     ((uint8_t)(0xFF & ~(0x03 << 6))) /* Force bits 00 (High) -> Compiles to 0x3F */
#define ISPR3_V11_TIM1_OVF_L3     ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  -> Compiles to 0xFF */

/* --- REGISTER ISPR4 (Vectors 12 to 15) --- */
#define ISPR4_V12_TIM1_CC_L1      ((uint8_t)(0xFF & ~(0x02 << 0))) /* Force bits 01 (Med)  */
#define ISPR4_V12_TIM1_CC_L2      ((uint8_t)(0xFF & ~(0x03 << 0))) /* Force bits 00 (High) */
#define ISPR4_V12_TIM1_CC_L3      ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR4_V13_TIM2_OVF_L1     ((uint8_t)(0xFF & ~(0x02 << 2))) /* Force bits 01 (Med)  */
#define ISPR4_V13_TIM2_OVF_L2     ((uint8_t)(0xFF & ~(0x03 << 2))) /* Force bits 00 (High) */
#define ISPR4_V13_TIM2_OVF_L3     ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR4_V14_TIM2_CC_L1      ((uint8_t)(0xFF & ~(0x02 << 4))) /* Force bits 01 (Med)  */
#define ISPR4_V14_TIM2_CC_L2      ((uint8_t)(0xFF & ~(0x03 << 4))) /* Force bits 00 (High) */
#define ISPR4_V14_TIM2_CC_L3      ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

/* --- REGISTER ISPR5 (Vectors 16 to 19) --- */
#define ISPR5_V17_UART1_TX_L1     ((uint8_t)(0xFF & ~(0x02 << 2))) /* Force bits 01 (Med)  */
#define ISPR5_V17_UART1_TX_L2     ((uint8_t)(0xFF & ~(0x03 << 2))) /* Force bits 00 (High) */
#define ISPR5_V17_UART1_TX_L3     ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR5_V18_UART1_RX_L1     ((uint8_t)(0xFF & ~(0x02 << 4))) /* Force bits 01 (Med)  */
#define ISPR5_V18_UART1_RX_L2     ((uint8_t)(0xFF & ~(0x03 << 4))) /* Force bits 00 (High) */
#define ISPR5_V18_UART1_RX_L3     ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR5_V19_I2C_L1          ((uint8_t)(0xFF & ~(0x02 << 6))) /* Force bits 01 (Med)  */
#define ISPR5_V19_I2C_L2          ((uint8_t)(0xFF & ~(0x03 << 6))) /* Force bits 00 (High) */
#define ISPR5_V19_I2C_L3          ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

/* --- REGISTER ISPR6 (Vectors 20 to 23) --- */
#define ISPR6_V22_TIM4_OVF_L1     ((uint8_t)(0xFF & ~(0x02 << 4))) /* Force bits 01 (Med)  */
#define ISPR6_V22_TIM4_OVF_L2     ((uint8_t)(0xFF & ~(0x03 << 4))) /* Force bits 00 (High) */
#define ISPR6_V22_TIM4_OVF_L3     ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#define ISPR6_V23_EEPROM_L1       ((uint8_t)(0xFF & ~(0x02 << 6))) /* Force bits 01 (Med)  */
#define ISPR6_V23_EEPROM_L2       ((uint8_t)(0xFF & ~(0x03 << 6))) /* Force bits 00 (High) */
#define ISPR6_V23_EEPROM_L3       ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

/* --- REGISTER ISPR7 (Vectors 24 to 27) --- */
#define ISPR7_V24_ADC1_L1         ((uint8_t)(0xFF & ~(0x02 << 0))) /* Force bits 01 (Med)  */
#define ISPR7_V24_ADC1_L2         ((uint8_t)(0xFF & ~(0x03 << 0))) /* Force bits 00 (High) */
#define ISPR7_V24_ADC1_L3         ((uint8_t)0xFF)                  /* Keeps bits 11 (Low)  */

#endif /* __ITC_ASM_H */