#ifndef __ADC_ASM_H
#define __ADC_ASM_H

// === ADC1 CHANNELS PACKED WITH SCHMITT MASK (0 bytes of Flash) ===
// Lower 4 bits = Channel Number, Upper 4 bits = Schmitt Trigger Mask (1 << Channel)
#define ADC_CH_0                  ((uint8_t)((0x01 << 4) | 0x00)) // Packed: 0x10
#define ADC_CH_1                  ((uint8_t)((0x02 << 4) | 0x01)) // Packed: 0x21
#define ADC_CH_2                  ((uint8_t)((0x04 << 4) | 0x02)) // Packed: 0x42
#define ADC_CH_3                  ((uint8_t)((0x08 << 4) | 0x03)) // Packed: 0x83
#define ADC_CH_4                  ((uint8_t)((0x10 << 4) | 0x04)) // Packed: 0x94

// === ADC1 PRESCALERS ===
#define ADC_PRESSEL_FCPU_D2       ((uint8_t)0x00)
#define ADC_PRESSEL_FCPU_D3       ((uint8_t)0x10)
#define ADC_PRESSEL_FCPU_D4       ((uint8_t)0x20)
#define ADC_PRESSEL_FCPU_D6       ((uint8_t)0x30)
#define ADC_PRESSEL_FCPU_D8       ((uint8_t)0x40)
#define ADC_PRESSEL_FCPU_D10      ((uint8_t)0x50)
#define ADC_PRESSEL_FCPU_D12      ((uint8_t)0x60)
#define ADC_PRESSEL_FCPU_D18      ((uint8_t)0x70)

#define ADC_INIT_CONTINUOUS(channelConfig, prescaler) \
	adcInitContinuous((channelConfig), (prescaler))
	
#define ADC_READ \
	adcRead()

// === PROTOTYPES (Direct assembly mappings) ===
void adcInitContinuous(uint8_t channelConfig, uint8_t prescaler);
uint16_t adcRead(void);

#endif /* __ADC_ASM_H */