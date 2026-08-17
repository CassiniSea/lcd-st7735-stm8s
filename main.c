/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 
a1 - button 3
a2 - button 4
a3 - motor (tim2_ch3)

b4(T) - 
b5(T) - led

c3 - button 1
c4 - term (ain2)
c5 - lcd sck
c6 - lcd mosi
c7 - button 2

d2 - lcd a0
d3 - lcd cs
d4 - heater control
d5 - uart tx
d6 - uart rx

										 3.3V        12V
										 |         |
										 |				 Heater
										 |         |
										 |     |---|
										 R2k   |<-?
										 |_____|---|
										 /         |
									 |/          |
		d4 --- R256 ---|           |
									 |\|         |
										-\         |
										 |         |
										___       ___
										 -         -
						 3.3V
						 |
						 R150
						 |
AIN2(C4) --- *
             |
						 Thermistor
						 |
						___
						 -
 */
 
#include "stm8s.h"
#include "uart.h"
#include "eeprom.h"
#include "math.h"

#define ST7735_PORT GPIOD
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
#define ST7735_GREY			0x630C

#define SCREEN_WIDTH       160
#define SCREEN_HEIGHT      128

#define BUTTONS_DEBOUNCE_LIMIT 20

#define EVENT_BUTTON_1_PRESSED  1
#define EVENT_BUTTON_2_PRESSED  (1<<1)
#define EVENT_BUTTON_3_PRESSED  (1<<2)
#define EVENT_BUTTON_4_PRESSED  (1<<3)
#define EVENT_BUTTON_5_PRESSED  (1<<4)
#define EVENT_GRAPH_UPDATE			(1<<5)
#define EVENT_HEATER_UPDATE			(1<<6)

#define UI_GRAPH_MAX_TEMP 320
#define UI_GRAPH_MIN_TEMP 200
#define UI_PANEL_HEIGHT 34
#define UI_GRAPH_WIDTH SCREEN_WIDTH
#define UI_GRAPH_HEIGHT (SCREEN_HEIGHT - UI_PANEL_HEIGHT - 1)
#define UI_GRAPH_TEMP_RANGE ((float)(UI_GRAPH_MAX_TEMP - UI_GRAPH_MIN_TEMP))
#define UI_GRAPH_HEIGHT_SCALE ((float)UI_GRAPH_HEIGHT / UI_GRAPH_TEMP_RANGE)
#define UI_GRAPH_GUIDE_INTERVAL 30
#define UI_GRAPH_GUIDE_COLOR ST7735_GREY
#define UI_PANEL_COLOR ST7735_BLUE
#define UI_TEXT_COLOR ST7735_WHITE
#define UI_TEXT_BG_COLOR ST7735_BLACK
#define UI_MARKER_COLOR ST7735_RED
#define UI_MODE_SELECT  0
#define UI_MODE_EDIT    1

#define FONT_WIDTH   5
#define FONT_HEIGHT  7

#define VARIABLES_NUMBER 5
#define VARIABLE_MAX_DIGITS 5

#define TIM1_FREQUENCY 500
#define GRAPH_UPDATE_DELAY_TIM1 1000
#define HEATER_UPDATE_DELAY_TIM1 500

#define THERM_R_25K 100000.0f
#define THERM_BETA 4410.0f
#define THERM_0K 273.15f
// 985
#define THERM_ADC_MAX 1023
#define THERM_R2 150.0f

#define I_MAX  5.0f

#define HEATER_PWM_PERIOD_MAX 976

enum VariableIndexes {
	VARIABLE_TEMP,
	VARIABLE_SPD,
	VARIABLE_KP,
	VARIABLE_KI,
	VARIABLE_KD
};

enum LetterIndexes {
	LETTER_T,
	LETTER_E,
	LETTER_M,
	LETTER_P,
	LETTER_S,
	LETTER_D,
	LETTER_K,
	LETTER_I
};

static const uint8_t ST7735_InitTable[] = {
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

static const uint8_t letters[8][FONT_HEIGHT] = {
    // T
    {
        0b11111,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100
    },
    // E
    {
        0b11111,
        0b10000,
        0b10000,
        0b11110,
        0b10000,
        0b10000,
        0b11111
    },
    // M
    {
        0b10001,
        0b11011,
        0b10101,
        0b10101,
        0b10001,
        0b10001,
        0b10001
    },
    // P
    {
        0b11110,
        0b10001,
        0b10001,
        0b11110,
        0b10000,
        0b10000,
        0b10000
    },
    // S
    {
        0b01111,
        0b10000,
        0b10000,
        0b01110,
        0b00001,
        0b00001,
        0b11110
    },
    // D
    {
        0b11110,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b11110
    },
    // K
    {
        0b10001,
        0b10010,
        0b10100,
        0b11000,
        0b10100,
        0b10010,
        0b10001
    },
    // I
    {
        0b11111,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b11111
    }
};

static const uint8_t digits[10][FONT_HEIGHT] = {
    // 0
    {
        0b01110,
        0b10001,
        0b10011,
        0b10101,
        0b11001,
        0b10001,
        0b01110
    },
    // 1
    {
        0b00100,
        0b01100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b01110
    },
    // 2
    {
        0b01110,
        0b10001,
        0b00001,
        0b00010,
        0b00100,
        0b01000,
        0b11111
    },
    // 3
    {
        0b11110,
        0b00001,
        0b00001,
        0b01110,
        0b00001,
        0b00001,
        0b11110
    },
    // 4
    {
        0b00010,
        0b00110,
        0b01010,
        0b10010,
        0b11111,
        0b00010,
        0b00010
    },
    // 5
    {
        0b11111,
        0b10000,
        0b10000,
        0b11110,
        0b00001,
        0b00001,
        0b11110
    },
    // 6
    {
        0b01110,
        0b10000,
        0b10000,
        0b11110,
        0b10001,
        0b10001,
        0b01110
    },
    // 7
    {
        0b11111,
        0b00001,
        0b00010,
        0b00100,
        0b01000,
        0b01000,
        0b01000
    },
    // 8
    {
        0b01110,
        0b10001,
        0b10001,
        0b01110,
        0b10001,
        0b10001,
        0b01110
    },
    // 9
    {
        0b01110,
        0b10001,
        0b10001,
        0b01111,
        0b00001,
        0b00001,
        0b01110
    }
};

static const uint8_t label_TEMP[] = {
	LETTER_T,
	LETTER_E,
	LETTER_M,
	LETTER_P
};

static const uint8_t label_SPD[] = {
	LETTER_S,
	LETTER_P,
	LETTER_D
};

static const uint8_t label_KP[] = {
	LETTER_K,
	LETTER_P
};

static const uint8_t label_KI[] = {
	LETTER_K,
	LETTER_I
};

static const uint8_t label_KD[] = {
	LETTER_K,
	LETTER_D
};

uint8_t graphValues[UI_GRAPH_WIDTH] = {0};

typedef struct {
	uint8_t current;
	uint8_t prev;
} VerticalGuidePosition_TypeDef;

typedef struct {
	uint16_t value;
	uint8_t x;
	uint8_t y;
	uint8_t valueIndent;
	const uint8_t* label;
	uint8_t label_len;
} Variable_TypeDef;

Variable_TypeDef variables[VARIABLES_NUMBER] = {
	{267, 10, SCREEN_HEIGHT - 2 * (FONT_HEIGHT + 3), 30, label_TEMP, 4}, 	// temp
	{2000, 10, SCREEN_HEIGHT - (FONT_HEIGHT + 3), 30, label_SPD, 3},			// spd
	{2, 90, SCREEN_HEIGHT - 3* (FONT_HEIGHT + 3), 20, label_KP, 2}, 			// kp
	{2, 90, SCREEN_HEIGHT - 2* (FONT_HEIGHT + 3), 20, label_KI, 2}, 			// ki
	{5, 90, SCREEN_HEIGHT - (FONT_HEIGHT + 3), 20, label_KD, 2}						// kd
};

// uint8_t scroll_position = SCREEN_HEIGHT - GRAPH_TOP - GRAPH_HEIGHT;
uint8_t isInitComplate = FALSE;
uint8_t buttonsDebounce = 0;
uint8_t programEvent = 0;

uint8_t ui_mode = UI_MODE_SELECT;

uint8_t selected_variable = VARIABLE_TEMP;
uint8_t selected_digit = 0;
uint8_t graphGuidePosition = 0;
uint16_t graphDelay = GRAPH_UPDATE_DELAY_TIM1;
uint16_t heaterDelay = HEATER_UPDATE_DELAY_TIM1;
uint16_t heaterPwmDuty;

float temperature;

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
		ST7735_CS_PIN | ST7735_A0_PIN,
		GPIO_MODE_OUT_PP_HIGH_FAST
	);
	
	SPI_Init(
    SPI_FIRSTBIT_MSB,
    SPI_BAUDRATEPRESCALER_2,
    SPI_MODE_MASTER,
    SPI_CLOCKPOLARITY_LOW,
    SPI_CLOCKPHASE_1EDGE,
    SPI_DATADIRECTION_1LINE_TX,
    SPI_NSS_SOFT,
    0x07
	);
	
	SPI_Cmd(ENABLE);
	
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

/*
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
*/

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

uint8_t diff(uint8_t a, uint8_t b) {
	if (a > b)
    return a - b;
	else
		return b - a;
}

void ST7735_FillRectByCoordinates(
    uint8_t x1,
    uint8_t y1,
    uint8_t x2,
    uint8_t y2,
    uint16_t color
)
{
    uint8_t start_x, start_y;
    uint8_t width, height;

    // 1. Находим начальную (левую верхнюю) точку по X и ширину
    if (x1 > x2) {
        start_x = x2;
        width = x1 - x2 + 1; // +1, так как координаты включительные
    } else {
        start_x = x1;
        width = x2 - x1 + 1;
    }

    // 2. Находим начальную (левую верхнюю) точку по Y и высоту
    if (y1 > y2) {
        start_y = y2;
        height = y1 - y2 + 1;
    } else {
        start_y = y1;
        height = y2 - y1 + 1;
    }

    // 3. Вызываем базовую функцию отрисовки прямоугольника
    ST7735_FillRect(start_x, start_y, width, height, color);
}

/*
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
*/

void ST7735_SetRotation(uint8_t rotation) {
    ST7735_CS_Low();

    ST7735_WriteCommand(0x36);

    switch (rotation) {
        case 0:
            ST7735_WriteData(0x00);
            break;

        case 1:
            ST7735_WriteData(0x60);
            break;

        case 2:
            ST7735_WriteData(0xC0);
            break;

        case 3:
            ST7735_WriteData(0xA0);
            break;
    }

    ST7735_CS_High();
}

void ST7735_DrawBitmap(
    uint8_t x,
    uint8_t y,
    const uint8_t *bitmap,
    uint8_t width,
    uint8_t height,
    uint16_t color,
    uint16_t bg_color
)
{
    uint8_t row;
    uint8_t column;
    uint8_t data;
    uint16_t pixel_color;
		
		ST7735_CS_Low();

    ST7735_SetAddressWindow(
        x,
        y,
        x + width - 1,
        y + height - 1
    );

    ST7735_WriteCommand(0x2C);   // RAMWR

    for (row = 0; row < height; row++)
    {
        data = bitmap[row];

        for (column = 0; column < width; column++)
        {
            if (data & (1 << (width - 1 - column)))
                pixel_color = color;
            else
                pixel_color = bg_color;

            ST7735_WriteData(pixel_color >> 8);
            ST7735_WriteData(pixel_color & 0xFF);
        }
    }

    ST7735_CS_High();
}


void UI_DrawText(
    uint8_t x,
    uint8_t y,
    const uint8_t *text,
    uint8_t length,
    uint16_t color,
    uint16_t bg_color
) {
    uint8_t i;

    for (i = 0; i < length; i++)
    {
        ST7735_DrawBitmap(
            x + i * 6,
            y,
            letters[text[i]],
            FONT_WIDTH,
            FONT_HEIGHT,
            color,
            bg_color
        );
    }
}

void UI_DrawNumber(
    uint8_t x,
    uint8_t y,
    uint16_t value,
    uint16_t color,
    uint16_t bg_color
)
{
    uint8_t digits_count;
    uint8_t digit;
    uint8_t position;
    uint16_t divisor;

    if (value >= 10000)
    {
        digits_count = 5;
    }
    else if (value >= 1000)
    {
        digits_count = 4;
    }
    else if (value >= 100)
    {
        digits_count = 3;
    }
    else if (value >= 10)
    {
        digits_count = 2;
    }
    else
    {
        digits_count = 1;
    }

    /*
     * Всегда выводим поле из 5 символов.
     * Ведущие позиции просто заливаем фоном.
     */
    for (position = 0; position < 5; position++)
    {
        if (position < (5 - digits_count))
        {
            ST7735_FillRect(
                x + position * 6,
                y,
                FONT_WIDTH,
                FONT_HEIGHT,
                bg_color
            );
        }
        else
        {
            divisor = 1;

            for (digit = 1; digit < (digits_count - (position - (5 - digits_count))); digit++)
            {
                divisor *= 10;
            }

            digit = value / divisor;
            value %= divisor;

            ST7735_DrawBitmap(
                x + position * 6,
                y,
                digits[digit],
                FONT_WIDTH,
                FONT_HEIGHT,
                color,
                bg_color
            );
        }
    }
}

void UI_DrawVariable(Variable_TypeDef variable) {
	UI_DrawText(
    variable.x,
    variable.y,
    variable.label,
    variable.label_len,
    UI_TEXT_COLOR,
    UI_TEXT_BG_COLOR
	);
	
	UI_DrawNumber(
    variable.x + variable.valueIndent,
    variable.y,
    variable.value,
    UI_TEXT_COLOR,
    UI_TEXT_BG_COLOR
	);
}

void UI_DrawVariables(void) {
	uint8_t variableIndex;
	
	for (variableIndex = 0; variableIndex < VARIABLES_NUMBER; variableIndex++) {
		UI_DrawVariable(variables[variableIndex]);		
	}
}

void UI_DrawMarker(void) {
	uint8_t variableIndex;
	uint8_t markerPosition;
	uint16_t color;
	Variable_TypeDef variable;
	
	for (variableIndex = 0; variableIndex < VARIABLES_NUMBER; variableIndex++) {
		if(variableIndex == selected_variable && ui_mode == UI_MODE_SELECT)
			color = UI_MARKER_COLOR;
		else 
			color = UI_PANEL_COLOR;
		
		variable = variables[variableIndex];
		
		ST7735_FillRect(
			variable.x - 5,
			variable.y + 2,
			2,
			3,
			color
		);
		
		
		for (markerPosition = 0; markerPosition < VARIABLE_MAX_DIGITS; markerPosition++) {
			if(variableIndex == selected_variable && markerPosition == selected_digit && ui_mode == UI_MODE_EDIT)
				color = UI_MARKER_COLOR;
			else
				color = UI_PANEL_COLOR;
				
			ST7735_FillRect(
				variable.x + variable.valueIndent + (FONT_WIDTH + 1) * markerPosition + 1,
				variable.y + FONT_HEIGHT,
				3,
				2,
				color
			);			
		}
	}
}

void variablesInit(void) {
	uint8_t variableIndex;
	
	for (variableIndex = 0; variableIndex < VARIABLES_NUMBER; variableIndex++) {
		variables[variableIndex].value = eepromReadU16(
			EEPROM_DEFAULT_ADDR + 2 * variableIndex,
			0
		);
	}	
}

void graphPushValue(uint8_t newValue) {
    uint8_t i;
		
		graphGuidePosition++;			
		if (graphGuidePosition >= UI_GRAPH_GUIDE_INTERVAL)
			graphGuidePosition = 0;

    // Идем с конца массива к началу
    // Элемент [i] принимает значение элемента [i - 1]
    for (i = UI_GRAPH_WIDTH - 1; i > 0; i--) {
        graphValues[i] = graphValues[i - 1];
    }

    // Записываем новое значение в самый первый элемент
    graphValues[0] = newValue;
}

uint8_t graphScaledValue(float graphValue) {
	float relativeValue;
	
	// Ограничиваем входное значение жестко рамками [200 ... 350]
  if (graphValue > UI_GRAPH_MAX_TEMP) graphValue = UI_GRAPH_MAX_TEMP;
  if (graphValue < UI_GRAPH_MIN_TEMP) graphValue = UI_GRAPH_MIN_TEMP;
	
	// Вычитаем минимальную температуру, чтобы график начинался от UI_GRAPH_MIN_TEMP
	relativeValue = graphValue - (float)UI_GRAPH_MIN_TEMP;
	
	return UI_GRAPH_HEIGHT - (uint8_t)(relativeValue * UI_GRAPH_HEIGHT_SCALE);
}

VerticalGuidePosition_TypeDef getVerticalGuidePosition(void) {
	static VerticalGuidePosition_TypeDef verticalGuidePosition = {0, 0};
	uint16_t tempValue = variables[VARIABLE_TEMP].value;
	
	if(tempValue > UI_GRAPH_MAX_TEMP)
		tempValue = UI_GRAPH_MAX_TEMP;
	
	verticalGuidePosition.prev = verticalGuidePosition.current;
	verticalGuidePosition.current = graphScaledValue(tempValue);
	
	return verticalGuidePosition;
}

void UI_DrawGraph(void) {
	uint8_t i;
	VerticalGuidePosition_TypeDef verticalGuidePosition = getVerticalGuidePosition();
	
	ST7735_FillRect(
		2,
		0,
		1,
		UI_GRAPH_HEIGHT,
		ST7735_BLACK
	);	
	for(i = graphGuidePosition; i < UI_GRAPH_WIDTH - 1; i += UI_GRAPH_GUIDE_INTERVAL) {
		ST7735_FillRect(
			UI_GRAPH_WIDTH - i + 1,
			0,
			1,
			UI_GRAPH_HEIGHT,
			ST7735_BLACK
		);
		ST7735_FillRect(
			UI_GRAPH_WIDTH - i,
			0,
			1,
			UI_GRAPH_HEIGHT,
			UI_GRAPH_GUIDE_COLOR
		);
	}	
	
	if(verticalGuidePosition.current != verticalGuidePosition.prev) {
		ST7735_FillRect(
			0,
			verticalGuidePosition.prev,
			SCREEN_WIDTH,
			1,
			ST7735_BLACK
		);
	}
	
	ST7735_FillRect(
		0,
		verticalGuidePosition.current,
		SCREEN_WIDTH,
		1,
		UI_GRAPH_GUIDE_COLOR
	);
	
	for(i = 0; i < UI_GRAPH_WIDTH - 2; i++) {
		ST7735_FillRectByCoordinates(
			UI_GRAPH_WIDTH - i,
			graphValues[i+1],
			UI_GRAPH_WIDTH - i,
			graphValues[i+2],
			ST7735_BLACK
		);
		
		if(graphValues[i] > 0 && graphValues[i] < UI_GRAPH_HEIGHT && graphValues[i+1] > 0 && graphValues[i+1] < UI_GRAPH_HEIGHT) {				
			ST7735_FillRectByCoordinates(
				UI_GRAPH_WIDTH - i,
				graphValues[i],
				UI_GRAPH_WIDTH - i,
				graphValues[i+1],
				ST7735_RED
			);
		}
	}
	
	UI_DrawNumber(
		40,
		SCREEN_HEIGHT - 3 * (FONT_HEIGHT + 3),
		(uint16_t)temperature,
		ST7735_GREEN,
		UI_PANEL_COLOR
	);
}

float getHeaterTemperature(void) {
	float rTherm = THERM_R2/(THERM_ADC_MAX/(float)ADC1_GetConversionValue() - 1);
	return (THERM_BETA * (THERM_0K + 25))/(THERM_BETA + (THERM_0K + 25) * log(rTherm / THERM_R_25K)) - THERM_0K;		
}

uint8_t getButtonsState(void) {
    uint8_t state = 0;

    if (GPIO_ReadInputPin(GPIOC, GPIO_PIN_3) == RESET)
      state |= EVENT_BUTTON_1_PRESSED;
				
		if (GPIO_ReadInputPin(GPIOC, GPIO_PIN_7) == RESET)
      state |= EVENT_BUTTON_2_PRESSED;

		if (GPIO_ReadInputPin(GPIOA, GPIO_PIN_1) == RESET)
      state |= EVENT_BUTTON_3_PRESSED;
			
		if (GPIO_ReadInputPin(GPIOA, GPIO_PIN_2) == RESET)
			state |= EVENT_BUTTON_4_PRESSED;

    return state;
}

void button1Routine(void) {
	uint16_t increment;
	
	//if(variables[VARIABLE_TEMP].value < 0xFFFF)
		//variables[VARIABLE_TEMP].value++;
		
	//UI_DrawVariable(variables[VARIABLE_TEMP]);
	
	if(ui_mode == UI_MODE_SELECT) {
		if(selected_variable > 0)
			selected_variable -= 1;
		else 
			selected_variable = VARIABLES_NUMBER - 1;
	}
	else if(ui_mode == UI_MODE_EDIT) {
    switch (selected_digit) {
			case 0: increment = 10000; break;
			case 1: increment = 1000;  break;
			case 2: increment = 100;   break;
			case 3: increment = 10;    break;
			default: increment = 1;    break;
    }
		
		if (variables[selected_variable].value > (0xFFFF - increment)) {
			variables[selected_variable].value = 0xFFFF;
		} else {
			variables[selected_variable].value += increment;
		}
		
		UI_DrawVariables();
	}
		
	UI_DrawMarker();
}

void button2Routine(void) {
	//GPIO_WriteReverse(GPIOB, GPIO_PIN_5);	
	//if(variables[VARIABLE_TEMP].value > 0)
		//variables[VARIABLE_TEMP].value--;
		
	//UI_DrawVariable(variables[VARIABLE_TEMP]);
	uint16_t decrement;
	
	if(ui_mode == UI_MODE_SELECT) {
		if(selected_variable < VARIABLES_NUMBER - 1)
			selected_variable += 1;
		else 
			selected_variable = 0;
	}
	else if(ui_mode == UI_MODE_EDIT) {
    switch (selected_digit) {
			case 0: decrement = 10000; break;
			case 1: decrement = 1000;  break;
			case 2: decrement = 100;   break;
			case 3: decrement = 10;    break;
			default: decrement = 1;    break;
    }
		
		if (decrement > variables[selected_variable].value) {
			variables[selected_variable].value = 0;
		} else {
			variables[selected_variable].value -= decrement;
		}
		
		UI_DrawVariables();
	}
		
	UI_DrawMarker();
}

void button3Routine(void) {
	//GPIO_WriteReverse(GPIOB, GPIO_PIN_5);	
	//if(variables[VARIABLE_TEMP].value < 0xFFFF - 100)
		//variables[VARIABLE_TEMP].value += 100;
		
	//UI_DrawVariable(variables[VARIABLE_TEMP]);
	if(selected_digit == 0) {
		ui_mode = UI_MODE_SELECT;
		eepromWriteU16(EEPROM_DEFAULT_ADDR + 2 * selected_variable, variables[selected_variable].value);
	}	else
		selected_digit--;
	
	UI_DrawMarker();
}

void button4Routine(void) {
	//GPIO_WriteReverse(GPIOB, GPIO_PIN_5);	
	//if(variables[VARIABLE_TEMP].value > 100)
		//variables[VARIABLE_TEMP].value -= 100;
		
	//UI_DrawVariable(variables[VARIABLE_TEMP]);
	if(ui_mode == UI_MODE_SELECT)
		ui_mode = UI_MODE_EDIT;
	else if(ui_mode == UI_MODE_EDIT) {		
		if(selected_digit < VARIABLE_MAX_DIGITS - 1)
			selected_digit++;
		else {
			variables[selected_variable].value = eepromReadU16(EEPROM_DEFAULT_ADDR + 2 * selected_variable, 0);
			UI_DrawVariables();
		}
	}
	
	UI_DrawMarker();
}

main() {
	uint8_t x;
	uint8_t test;
	
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1|CLK_PRESCALER_CPUDIV1);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
	
	GPIO_Init(GPIOB, GPIO_PIN_4 | GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
	//GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);

	ST7735_Init();
	ST7735_SetRotation(1);
	/*	
	ST7735_SetScrollArea(
    10,
    120,
    30
	);
	*/
	
	uartInit();
	
	EXTI_SetExtIntSensitivity(
		EXTI_PORT_GPIOA,
		EXTI_SENSITIVITY_FALL_ONLY
	);	
	GPIO_Init(GPIOA, GPIO_PIN_1 | GPIO_PIN_2, GPIO_MODE_IN_PU_IT);
	
	EXTI_SetExtIntSensitivity(
		EXTI_PORT_GPIOC,
		EXTI_SENSITIVITY_FALL_ONLY
	);	
	GPIO_Init(GPIOC, GPIO_PIN_3 | GPIO_PIN_7, GPIO_MODE_IN_PU_IT);

	TIM4_TimeBaseInit(TIM4_PRESCALER_128, 125); // 1ms
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_Cmd(ENABLE);
	
	ADC1_Init(	ADC1_CONVERSIONMODE_CONTINUOUS,
							ADC1_CHANNEL_2,
							ADC1_PRESSEL_FCPU_D4,
							ADC1_EXTTRIG_TIM,
							DISABLE,
							ADC1_ALIGN_RIGHT,
							ADC1_SCHMITTTRIG_CHANNEL3,
							DISABLE);
	ADC1_Cmd(ENABLE);
	ADC1_StartConversion();
	
	TIM1_TimeBaseInit(16000, TIM1_COUNTERMODE_UP, 1000/TIM1_FREQUENCY, 0);
	TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
	
	TIM2_TimeBaseInit(	TIM2_PRESCALER_16384,
											HEATER_PWM_PERIOD_MAX);
	TIM2_OC1Init(				TIM2_OCMODE_PWM2,
											TIM2_OUTPUTSTATE_ENABLE,
											0,
											TIM2_OCPOLARITY_HIGH);
	TIM2_OC1PreloadConfig(ENABLE); 
	TIM2_ARRPreloadConfig(ENABLE);
	
	enableInterrupts();
	
	ST7735_FillRect(
		0,
		0,
		SCREEN_WIDTH,
		SCREEN_HEIGHT - UI_PANEL_HEIGHT,
		ST7735_BLACK
	);
	
	ST7735_FillRect(
    0,
    SCREEN_HEIGHT - UI_PANEL_HEIGHT,
    SCREEN_WIDTH,
    UI_PANEL_HEIGHT,
    UI_PANEL_COLOR
	);
	
	/*
	ST7735_FillRect(0,   0,   40, 40, ST7735_RED);
	ST7735_FillRect(98,  0,   40, 40, ST7735_GREEN);
	ST7735_FillRect(0,   120, 40, 40, ST7735_BLUE);
	ST7735_FillRect(88,  120, 40, 40, ST7735_WHITE);
	/*
	
	/*	
	ST7735_FillRect(
    0,
    GRAPH_TOP + GRAPH_HEIGHT,
    GRAPH_WIDTH,
    SCREEN_HEIGHT - GRAPH_TOP - GRAPH_HEIGHT,
    ST7735_GREEN
	);
	*/
	
	/*
	for (x = 0; x < SCREEN_WIDTH; x += 10)	{
		ST7735_FillRect(
			x,
			GRAPH_TOP,
			5,
			GRAPH_HEIGHT,
			ST7735_RED
		);
	}
	*/
	
	variablesInit();
	
	UI_DrawVariables();
	UI_DrawMarker();
	
	isInitComplate = TRUE;
	
	TIM2_Cmd(ENABLE);
	TIM1_Cmd(ENABLE);	
	
	while (1) {
		uint16_t heaterMarkerColor = UI_PANEL_COLOR;
		
		if(programEvent & EVENT_BUTTON_1_PRESSED) {
			button1Routine();
			programEvent &= ~EVENT_BUTTON_1_PRESSED;
		}
		
		if(programEvent & EVENT_BUTTON_2_PRESSED) {
			button2Routine();
			programEvent &= ~EVENT_BUTTON_2_PRESSED;
		}
		
		if(programEvent & EVENT_BUTTON_3_PRESSED) {
			button3Routine();
			programEvent &= ~EVENT_BUTTON_3_PRESSED;
		}
		
		if(programEvent & EVENT_BUTTON_4_PRESSED) {
			button4Routine();
			programEvent &= ~EVENT_BUTTON_4_PRESSED;
		}
		
		if(programEvent & EVENT_GRAPH_UPDATE) {
			UI_DrawGraph();			
			programEvent &= ~EVENT_GRAPH_UPDATE;
		}
		
		if(GPIO_ReadInputPin(GPIOD, GPIO_PIN_4) == RESET)
			heaterMarkerColor = ST7735_RED;

		ST7735_FillRect(
			10,
			SCREEN_HEIGHT - 3 * (FONT_HEIGHT + 3),
			FONT_HEIGHT,
			FONT_HEIGHT,
			heaterMarkerColor
		);
	}

}

@far @interrupt void tim1UpdateInterrupt(void) {	
	if (buttonsDebounce) {
    buttonsDebounce--;

    if (buttonsDebounce == 0) {
			programEvent |= getButtonsState();
    }
	}	
	
	heaterDelay++;
	if(heaterDelay > HEATER_UPDATE_DELAY_TIM1) {
		static float pid_prev_error = 0.0f;
		static float pid_integral = 0.0f;
		float error, p_term, d_term, output;
    float kp = (float)variables[VARIABLE_KP].value;
    float ki = (float)variables[VARIABLE_KI].value;
    float kd = (float)variables[VARIABLE_KD].value;
		
		heaterDelay = 0;

		temperature = getHeaterTemperature();
		
    // Считаем ошибку		
    error = (float)variables[VARIABLE_TEMP].value - temperature;
		
		// Пропорциональная составляющая
    p_term = kp * error;
		
		// Интегральная составляющая
    //pid_integral += ki * error;		
		if (error < 10.0f && error > -10.0f) {
				// Интегратор работает только в пределах +-10 градусов от цели
				pid_integral += ki * error; 
		} 
		else {
				// Пока мы далеко от цели, сбрасываем интегратор, чтобы он не копил мусор
				pid_integral = 0.0f; 
		}
    // Анти-виндэп (Anti-windup): ограничиваем интеграл, чтобы он не "разгонялся"
    if (pid_integral > HEATER_PWM_PERIOD_MAX / 2) pid_integral = HEATER_PWM_PERIOD_MAX / 2;
    else if (pid_integral < 0.0f) pid_integral = 0.0f;
		
		// Дифференциальная составляющая
    d_term = kd * (error - pid_prev_error);
    pid_prev_error = error;
		
		// Суммируем выходное значение
    output = p_term + pid_integral + d_term;
		
		// Ограничиваем выход под рамки ARR таймера (0 - 1250)
    heaterPwmDuty = 0;
    if (output >= (float)HEATER_PWM_PERIOD_MAX) {
        heaterPwmDuty = HEATER_PWM_PERIOD_MAX + 1;
    } else if (output <= 0.0f) {
        heaterPwmDuty = 0;
    } else {
        heaterPwmDuty = (uint16_t)output;
    }
		
		// Обновляем ШИМ регистра таймера
    TIM2_SetCompare1(heaterPwmDuty);
	}
	
	graphDelay++;
	if(graphDelay > GRAPH_UPDATE_DELAY_TIM1) {
		graphDelay = 0;
		programEvent |= EVENT_GRAPH_UPDATE;
		graphPushValue(graphScaledValue(temperature));
	}
	
	TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
	//graphPushValue();
	
	/*	
	scroll_position++;
	if (scroll_position >= SCREEN_HEIGHT - GRAPH_TOP) {
		scroll_position = SCREEN_HEIGHT - GRAPH_TOP - GRAPH_HEIGHT;
	}
	
	if(isInitComplate == TRUE) {	
		ST7735_SetScrollStart(scroll_position);
	}
	*/
}

@far @interrupt void tim4UpdateInterrupt(void) {
	TIM4_ClearITPendingBit(TIM1_IT_UPDATE);
	
}

@far @interrupt void gpioaExtiInterrupt(void) {
	buttonsDebounce = BUTTONS_DEBOUNCE_LIMIT;
}

@far @interrupt void gpiocExtiInterrupt(void) {	
	buttonsDebounce = BUTTONS_DEBOUNCE_LIMIT;
}

void uartReceiveByte(uint8_t byte) {
	uartSendByte(byte);
//	ST7735_SetScrollStart((uint16_t)byte);
}