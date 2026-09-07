	switch	.text
	
; Hardware Registers for Flash/EEPROM control
FLASH_DUKR:   equ $5064    ; Data EEPROM unprotection register
FLASH_IAPSR:  equ $505F    ; Internal memory status register

;=============================================================================
; Function:    _eepromReadU16
; Description: Calculates EEPROM address from index (0x4000 + index * 2) 
;              and reads a 16-bit big-endian value.
; C Prototype: uint16_t eepromReadByIndex(uint8_t index, uint16_t defaultValue);
;=============================================================================
	xdef	_eepromReadU16
_eepromReadU16:
	;--- Step 1: Calculate exact 16-bit memory address ---
	sll	a		; Shift index left by 1 (effectively: index * 2)
	clrw	x		; Clear 16-bit register X
	ld	xl, a		; Move shifted index into XL register (X = index * 2)
	addw	x, #$4000	; Add base EEPROM address (X now holds exact address)

	;--- Step 2: Read data from calculated address ---
	ld	a, (0, x)	; Load High byte from EEPROM into register A
	clrw	y		; Clear 16-bit register Y (YL becomes 0)
	exg	a, yl		; Move High byte into YL (A now becomes 0)
	ld	a, (1, x)	; Load Low byte from EEPROM into register A
	clrw	x		; Clear X register to prepare return value
	ld	xl, a		; Place Low byte from A into XL register
	exg	a, yl		; Move High byte from YL back to A
	ld	xh, a		; Move High byte from A into XH register. X now holds exact 'value'.

	;--- Step 3: Validation checks ---
	cpw	x, #$FFFF	; Compare 16-bit X register with 0xFFFF (blank memory)
	jreq	_eeprom_read_default ; If blank, branch to default return routine
	cpw	x, #$0000	; Compare 16-bit X register with 0x0000 (empty memory)
	jreq	_eeprom_read_default ; If empty, branch to default return routine
	ret			; Return successfully (X contains valid EEPROM data)

_eeprom_read_default:
	;--- Step 4: Load defaultValue from stack ---
	ld	a, (3, sp)	; Fetch High byte of defaultValue from stack into register A
	ld	xh, a		; Move High byte into XH register (This instruction is fully valid!)
	ld	a, (4, sp)	; Fetch Low byte of defaultValue from stack into register A
	ld	xl, a		; Move Low byte into XL register. X now holds 'defaultValue'.
	ret			; Return back to C caller


	switch	.text

;=============================================================================
; Function:    _eepromWriteU16
; Description: Calculates EEPROM address from index (0x4000 + index * 2),
;              unlocks memory, writes 16-bit data, and locks it back.
; C Prototype: void eepromWriteByIndex(uint8_t index, uint16_t data);
; Inputs:      A      = 8-bit variable index (0, 1, 2...). Passed via register A.
;              (3,sp) = 16-bit data to write (High byte).
;              (4,sp) = 16-bit data to write (Low byte).
;=============================================================================
	xdef	_eepromWriteU16
_eepromWriteU16:
	;--- Step 1: Calculate exact 16-bit memory address ---
	sll	a		; Shift index left by 1 (effectively: index * 2)
	clrw	x		; Clear 16-bit register X
	ld	xl, a		; Move shifted index into XL register (X = index * 2)
	addw	x, #$4000	; Add base EEPROM address (X now holds exact address)
	ldw	y, x		; Backup our target memory address into register Y

	;--- Step 2: Unlock DATA EEPROM ---
	mov	FLASH_DUKR, #$AE  ; Write first hardware unprotection key
	mov	FLASH_DUKR, #$56  ; Write second hardware unprotection key

_eeprom_wait_unlock:
	btjf	FLASH_IAPSR, #3, _eeprom_wait_unlock ; Wait until PUL bit (bit 3) becomes 1

	;--- Step 3: Write 16-bit data (Big-Endian format) ---
	ld	a, (3, sp)	; Fetch High byte of data from stack
	ld	(0, y), a	; Write High byte directly to memory address [Y]
	ld	a, (4, sp)	; Fetch Low byte of data from stack
	ld	(1, y), a	; Write Low byte to next memory address [Y + 1]

	;--- Step 4: Wait for operation complete & Lock back ---
_eeprom_wait_eop:
	btjf	FLASH_IAPSR, #2, _eeprom_wait_eop ; Wait until EOP bit (bit 2) becomes 1
	bres	FLASH_IAPSR, #3	; Clear PUL bit (bit 3) to lock write operations back
	ret			; Return back to C caller

	end

