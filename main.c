/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
 
#include "stm8s.h"
#include "uart.h"

#define ST7735_PORT GPIOD
#define ST7735_RESET_PIN GPIO_PIN_4
#define ST7735_CS_PIN GPIO_PIN_3
#define ST7735_A0_PIN GPIO_PIN_2
#define ST7735_DELAY 0xFF
#define ST7735_BLACK    0x0000
#define ST7735_RED      0xF800
#define ST7735_GREEN    0x07E0
#define ST7735_BLUE     0x001F
#define ST7735_WHITE    0xFFFF
#define ST7735_YELLOW   0xFFE0
#define ST7735_CYAN     0x07FF
#define ST7735_MAGENTA  0xF81F

#define SCREEN_WIDTH       128
#define SCREEN_HEIGHT      160

#define GRAPH_TOP          30
#define GRAPH_HEIGHT       120

#define GRAPH_X            0
#define GRAPH_WIDTH        128

static const uint8_t ST7735_InitTable[] =
{
	0x01, 0,              // SWRESET
	ST7735_DELAY, 150,

	0x11, 0,              // SLPOUT
	ST7735_DELAY, 150,

	0xB1, 3, 0x01, 0x2C, 0x2D,
	0xB2, 3, 0x01, 0x2C, 0x2D,
	0xB3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,

	0xB4, 1, 0x07,

	0xC0, 3, 0xA2, 0x02, 0x84,
	0xC1, 1, 0xC5,
	0xC2, 2, 0x0A, 0x00,
	0xC3, 2, 0x8A, 0x2A,
	0xC4, 2, 0x8A, 0xEE,
	0xC5, 1, 0x0E,

	0x20, 0,

	0x36, 1, 0xC0,

	0x3A, 1, 0x05,

	0xE0, 16,
	0x02, 0x1C, 0x07, 0x12,
	0x37, 0x32, 0x29, 0x2D,
	0x29, 0x25, 0x2B, 0x39,
	0x00, 0x01, 0x03, 0x10,

	0xE1, 16,
	0x03, 0x1D, 0x07, 0x06,
	0x2E, 0x2C, 0x29, 0x2D,
	0x2E, 0x2E, 0x37, 0x3F,
	0x00, 0x00, 0x02, 0x10,

	0x13, 0,
	ST7735_DELAY, 10,

	0x29, 0,
	ST7735_DELAY, 100
};

uint8_t scroll_position = SCREEN_HEIGHT - GRAPH_TOP - GRAPH_HEIGHT;
uint8_t isInitComplate = FALSE;

void SPI_SendByte(uint8_t data) {
	SPI_SendData(data);

	while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET) {
	}

	while (SPI_GetFlagStatus(SPI_FLAG_BSY) == SET) {
	}
}

void ST7735_WriteCommand(uint8_t command) {
	GPIO_WriteLow(ST7735_PORT, ST7735_A0_PIN);

	SPI_SendByte(command);
}

void ST7735_WriteData(uint8_t data) {
	GPIO_WriteHigh(ST7735_PORT, ST7735_A0_PIN);

	SPI_SendByte(data);
}

void ST7735_CS_Low(void) {
	GPIO_WriteLow(ST7735_PORT, ST7735_CS_PIN);
}

void ST7735_CS_High(void) {
	GPIO_WriteHigh(ST7735_PORT, ST7735_CS_PIN);
}

void ST7735_RESET_Low(void) {
	GPIO_WriteLow(ST7735_PORT, ST7735_RESET_PIN);
}

void ST7735_RESET_High(void) {
	GPIO_WriteHigh(ST7735_PORT, ST7735_RESET_PIN);
}

void delay_ms(uint16_t ms) {
	uint16_t i;

	while (ms--) {
		for (i = 0; i < 1000; i++) {
			nop();
		}
	}
}

void ST7735_RunInitTable(void) {
	uint8_t i;
	uint8_t j;
	uint8_t command;
	uint8_t count;

	i = 0;

	while (i < sizeof(ST7735_InitTable)) {
		command = ST7735_InitTable[i++];

		if (command == ST7735_DELAY) {
			delay_ms(ST7735_InitTable[i++]);
			continue;
		}

		ST7735_WriteCommand(command);

		count = ST7735_InitTable[i++];

		for (j = 0; j < count; j++)	{
			ST7735_WriteData(ST7735_InitTable[i++]);
		}
	}
}

void ST7735_Init(void) {
	GPIO_Init(
		ST7735_PORT,
		ST7735_CS_PIN | ST7735_A0_PIN | ST7735_RESET_PIN,
		GPIO_MODE_OUT_PP_HIGH_FAST
	);
	
	SPI_Init(
    SPI_FIRSTBIT_MSB,
    SPI_BAUDRATEPRESCALER_8,
    SPI_MODE_MASTER,
    SPI_CLOCKPOLARITY_LOW,
    SPI_CLOCKPHASE_1EDGE,
    SPI_DATADIRECTION_2LINES_FULLDUPLEX,
    SPI_NSS_SOFT,
    0x07
	);
	
	SPI_Cmd(ENABLE);
	
	ST7735_RESET_Low();
	delay_ms(20);
	ST7735_RESET_High();
	delay_ms(120);

	ST7735_CS_Low();

	ST7735_RunInitTable();

	ST7735_CS_High();
}

void ST7735_SetAddressWindow(
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1
)
{
    ST7735_WriteCommand(0x2A);

    ST7735_WriteData(0x00);
    ST7735_WriteData(x0);

    ST7735_WriteData(0x00);
    ST7735_WriteData(x1);


    ST7735_WriteCommand(0x2B);

    ST7735_WriteData(0x00);
    ST7735_WriteData(y0);

    ST7735_WriteData(0x00);
    ST7735_WriteData(y1);


    ST7735_WriteCommand(0x2C);
}

void ST7735_FillScreen(uint16_t color)
{
    uint16_t i;
    uint16_t pixel_count;

    ST7735_CS_Low();

    ST7735_SetAddressWindow(0, 0, 127, 159);

    pixel_count = 128 * 160;

    for (i = 0; i < pixel_count; i++)
    {
        ST7735_WriteData(color >> 8);
        ST7735_WriteData(color & 0xFF);
    }

    ST7735_CS_High();
}

void ST7735_FillRect(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    uint16_t color
)
{
    uint16_t i;
    uint16_t pixel_count;

    ST7735_CS_Low();

    ST7735_SetAddressWindow(
        x,
        y,
        x + width - 1,
        y + height - 1
    );

    pixel_count = (uint16_t)width * height;

    for (i = 0; i < pixel_count; i++)
    {
        ST7735_WriteData(color >> 8);
        ST7735_WriteData(color & 0xFF);
    }

    ST7735_CS_High();
}

void ST7735_SetScrollArea(
    uint16_t top_fixed,
    uint16_t scroll_height,
    uint16_t bottom_fixed
)
{
    ST7735_CS_Low();

    ST7735_WriteCommand(0x33);       // VSCRDEF

    ST7735_WriteData(top_fixed >> 8);
    ST7735_WriteData(top_fixed & 0xFF);

    ST7735_WriteData(scroll_height >> 8);
    ST7735_WriteData(scroll_height & 0xFF);

    ST7735_WriteData(bottom_fixed >> 8);
    ST7735_WriteData(bottom_fixed & 0xFF);

    ST7735_CS_High();
}

void ST7735_SetScrollStart(uint16_t position)
{
    ST7735_CS_Low();

    ST7735_WriteCommand(0x37);       // VSCSAD

    ST7735_WriteData(position >> 8);
    ST7735_WriteData(position & 0xFF);

    ST7735_CS_High();
}

main() {
	uint8_t x;
	
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1|CLK_PRESCALER_CPUDIV1);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
	
//	ST7735_CS_Low();	
//		ST7735_WriteCommand(0xAA);
//		ST7735_WriteData(0x55);		
//	ST7735_CS_High();

	TIM1_TimeBaseInit(16000, TIM1_COUNTERMODE_UP, 100, 0);
	TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
	TIM1_Cmd(ENABLE);
	
	GPIO_Init(
		GPIOB,
		GPIO_PIN_5,
		GPIO_MODE_OUT_PP_HIGH_FAST
	);

	ST7735_Init();
	
	ST7735_SetScrollArea(
    10,
    120,
    30
	);
	
	uartInit();
	
	enableInterrupts();
	
	ST7735_FillScreen(ST7735_BLACK);

//	ST7735_FillRect(0,   0,   40, 40, ST7735_RED);
//	ST7735_FillRect(88,  0,   40, 40, ST7735_GREEN);
//	ST7735_FillRect(0,   120, 40, 40, ST7735_BLUE);
//	ST7735_FillRect(88,  120, 40, 40, ST7735_WHITE);

	ST7735_FillRect(
			0,
			0,
			128,
			30,
			ST7735_BLUE
	);
	
	ST7735_FillRect(
    0,
    GRAPH_TOP,
    GRAPH_WIDTH,
    GRAPH_HEIGHT - 10,
    ST7735_MAGENTA
	);
	
	ST7735_FillRect(
    0,
    GRAPH_TOP + GRAPH_HEIGHT,
    GRAPH_WIDTH,
    SCREEN_HEIGHT - GRAPH_TOP - GRAPH_HEIGHT,
    ST7735_GREEN
	);
	
	for (x = 0; x < SCREEN_WIDTH; x += 10)	{
		ST7735_FillRect(
			x,
			GRAPH_TOP,
			5,
			GRAPH_HEIGHT,
			ST7735_RED
		);
	}
	
	isInitComplate = TRUE;
	
	while (1) {
//		ST7735_CS_Low();
//			delay_ms(100);
//		ST7735_CS_High();		
	}
}

@far @interrupt void tim1UpdateInterrupt(void) {
	TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
	GPIO_WriteReverse(GPIOB, GPIO_PIN_5);
	
	scroll_position++;
	if (scroll_position >= SCREEN_HEIGHT - GRAPH_TOP) {
		scroll_position = SCREEN_HEIGHT - GRAPH_TOP - GRAPH_HEIGHT;
	}
	
	if(isInitComplate == TRUE) {	
		ST7735_SetScrollStart(scroll_position);
	}

}

void uartReceiveByte(uint8_t byte) {
	uartSendByte(byte);
	ST7735_SetScrollStart((uint16_t)byte);
}