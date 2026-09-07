; === ADC1 HARDWARE REGISTERS FOR ASM ===
ADC1_CSR:   equ $5400    ; Control/Status register (Channel select, EOC flag)
ADC1_CR1:   equ $5401    ; Control register 1 (Prescaler, Cont. mode, ADON)
ADC1_CR2:   equ $5402    ; Control register 2 (Alignment, Ext. trigger)
ADC1_DRH:   equ $5404    ; Data register High
ADC1_DRL:   equ $5405    ; Data register Low
ADC1_TDRL:  equ $5407    ; Schmitt trigger disable register Low (Channels 0-7)

	switch	.text

;=============================================================================
; Function:    _adcInitContinuous
; Description: Initializes ADC1 in Continuous mode using packed register X.
;              Extracts channel number, pre-calculated Schmitt trigger mask,
;              and clock prescaler. No stack usage.
; Inputs:      X = Packed values from Cosmic C: 
;                  XH (high byte) = channel_config (Upper 4 bits: Schmitt, Lower 4 bits: Channel)
;                  XL (low byte)  = Clock prescaler mask (e.g., 0x20)
;=============================================================================
	xdef	_adcInitContinuous
_adcInitContinuous:
	;--- Step 1: Set Channel (ADC1_CSR) ---
	ld	a, xh		; Load channel_config into Accumulator A
	and	a, #$0F		; Clear upper 4 bits to isolate pure Channel Number (e.g., 0x02)
	ld	ADC1_CSR, a	; Write channel index straight into CSR register

	;--- Step 2: Disable Schmitt Trigger for the Active Channel (ADC1_TDRL) ---
	ld	a, xh		; Reload channel_config into Accumulator A
	and	a, #$F0		; Clear lower 4 bits to isolate Pre-calculated Schmitt Mask
	swap	a		; Swap nibbles to move mask to lower 4 bits (e.g., 0x40 -> 0x04)
	ld	ADC1_TDRL, a	; Write mask directly to TDRL to disable Schmitt trigger

	;--- Step 3: Configure Data Alignment (ADC1_CR2) ---
	mov	ADC1_CR2, #$08	; Set ALIGN=1 (Right alignment), EXTTRIG=0 (Disabled)

	;--- Step 4: Configure Prescaler & Power On (ADC1_CR1) ---
	ld	a, xl		; Fetch prescaler mask from XL register
	or	a, #$03		; Combine with CONT=1 (Continuous mode) and ADON=1 (First power on)
	ld	ADC1_CR1, a	; Write configuration to CR1 register. ADC wakes up.
	
	;--- Step 5: Start Conversion ---
	bset	ADC1_CR1, #0	; Set ADON bit again to start continuous conversion loop
	ret			; Return back to C caller

;=============================================================================
; Function:    _adcRead
; Description: Reads the latest 10-bit conversion result from ADC1 registers.
;              Optimized to read the 16-bit word directly into register X.
; C Prototype: uint16_t adcRead(void);
; Outputs:     X      = 16-bit right-aligned conversion result (0 to 1023)
;=============================================================================
	xdef	_adcRead
_adcRead:
	ldw	x, ADC1_DRH	; Direct 16-bit load: XH = ADC1_DRH, XL = ADC1_DRL (Perfect & fast!)
	ret			; Return back to C caller (Result is already inside register X)

	end
