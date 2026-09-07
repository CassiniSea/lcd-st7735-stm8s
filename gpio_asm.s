	switch	.text

;=============================================================================
; Function:    _gpioInitOutput
; Description: Safely sets specific port pins to Output PP, High, 10MHz Fast.
;              Preserves settings of all other pins on the same port.
; Inputs:      X      = 16-bit hardware address of port ODR register.
;              (3,sp) = 8-bit pin mask (passed from C code via stack).
; Outputs:     None. Modifies port hardware registers.
;=============================================================================
xdef	_gpioInitOutput
_gpioInitOutput:
	ld	a, (3, sp)	; Fetch pin_mask from stack into register A

	;--- Configure ODR (Output Data Register, Offset +0) ---
	or	a, (0, x)	; OR mask with current ODR to keep old High pins
	ld	(0, x), a	; Write back to ODR (Target pins transition to High)

	;--- Configure DDR (Data Direction Register, Offset +2) ---
	ld	a, (3, sp)	; Reload clean pin_mask from stack into register A
	or	a, (2, x)	; OR mask with current DDR to keep old Output pins
	ld	(2, x), a	; Write back to DDR (Target pins switch to Output)

	;--- Configure CR1 (Control Register 1, Offset +3) ---
	ld	a, (3, sp)	; Reload clean pin_mask from stack into register A
	or	a, (3, x)	; OR mask with current CR1 to keep old Push-Pull pins
	ld	(3, x), a	; Write back to CR1 (Target pins become Push-Pull)

	;--- Configure CR2 (Control Register 2, Offset +4) ---
	ld	a, (3, sp)	; Reload clean pin_mask from stack into register A
	or	a, (4, x)	; OR mask with current CR2 to keep old Fast speed pins
	ld	(4, x), a	; Write back to CR2 (Target pins set to 10MHz Fast)
	ret			; Return back to the C program execution


;=============================================================================
; Function:    _gpioInitInput
; C Prototype: void gpioInitInput(void* gpioOdrAddr, uint8_t pinMask);
;=============================================================================
	xdef	_gpioInitInput
_gpioInitInput:
	ld	a, (3, sp)	; Fetch pinMask from stack into register A
	cpl	a		; Invert mask bits (Target bits become 0, others 1)
	and	a, (2, x)	; Clear only target bits in current DDR register
	ld	(2, x), a	; Save back to DDR (Target pins switch to Input)

	ld	a, (3, sp)	; Reload clean pinMask from stack into register A
	or	a, (3, x)	; Set chosen bits to 1 in current CR1 value
	ld	(3, x), a	; Save back to CR1 (Enable internal Pull-Up resistors)
	ret			; Return back to the C program execution

	end