#include "uart.h"

#ifdef UART_SEND_STRING_ASYNC_ENABLE
char* __uartStrPtr;
#endif

#ifdef UART_RECEIVE_STRING_ENABLE
char __uartBuffer[UART_MAX_STRING_LENGTH];
uint8_t __uartBufferIndex = 0;
#endif

void uartInit(void) {
	UART1_DeInit();
	UART1_Init(	57600,
							UART1_WORDLENGTH_8D,
							UART1_STOPBITS_1,
							UART1_PARITY_NO,
							UART1_SYNCMODE_CLOCK_DISABLE,
							UART1_MODE_TXRX_ENABLE);
	#if defined(UART_RECEIVE_STRING_ENABLE) || defined(UART_RECEIVE_BYTE_ENABLE)
		UART1_ITConfig(	UART1_IT_RXNE, ENABLE);
	#endif
	UART1_Cmd(ENABLE);
}

void uartStringReceived(char* str) {

}

void uartSendByte(uint8_t byte) {
	while(!UART1_GetFlagStatus(UART1_FLAG_TXE));
	UART1_SendData8(byte);
}

void uartSendString(char* str) {
	while(*str) {
		uartSendByte(*str++);
	}
}

uint8_t stringToNum(const char *str) {
  uint8_t num = 0;
  while (*str != '\0') {
		if (*str < '0' || *str > '9') {
			return 0;
		}
		num = num * 10 + (*str - '0');
		str++;
	}
  return num;
}

void intToStr(int32_t num, char* str) {
    char buffer[12];
    uint8_t i = 0, j = 0;
    int8_t isNegative = 0;
    
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }
    
    do {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);
    
    if (isNegative) {
        buffer[i++] = '-';
    }
    
    for (j = 0; j < i; j++) {
        str[j] = buffer[i - j - 1];
    }
    str[i] = '\0';
}

void floatToStr(float num, char* str, uint8_t precision) {
	uint8_t i = 0;
  int32_t intPart = (int32_t)num;
  float fracPart = num - (float)intPart;
	
	if(num < 0) {
		fracPart = -fracPart;
	}
    
  intToStr(intPart, str);
	
	if(precision != 0) {
		while (*str) str++;
		
		*str++ = '.';
		 
		for (i = 0; i < precision; i++) {
			fracPart *= 10;
		}
			
		intToStr((int32_t)(fracPart + 0.5), str);
	}
}

#ifdef UART_SEND_STRING_ASYNC_ENABLE
void uartSendStringAsync(char* str) {
	__uartStrPtr = str;
	UART1_ITConfig(UART1_IT_TXE, ENABLE);
}

void uartTxComplete(void) {
	if (*__uartStrPtr) {
		UART1_SendData8(*__uartStrPtr++);
	}
	else {
		UART1_ITConfig(UART1_IT_TXE, DISABLE);
	}
}
#endif

#ifdef UART_RECEIVE_STRING_ENABLE
void uartReceiveByte(uint8_t byte) {
	if(byte == '\r' || __uartBufferIndex >= UART_MAX_STRING_LENGTH - 1) {
		__uartBuffer[__uartBufferIndex] = '\0';
		__uartBufferIndex = 0;
		uartStringReceived(__uartBuffer);
	}
	else {
		__uartBuffer[__uartBufferIndex++] = byte;
	}
}
#endif

#if defined(UART_RECEIVE_STRING_ENABLE) || defined(UART_RECEIVE_BYTE_ENABLE)
	INTERRUPT void UART_RECEIVE8_INTERRUPT_VECTOR(void)	{
		UART1_ClearITPendingBit(UART1_IT_RXNE);
		uartReceiveByte(UART1_ReceiveData8());
	}
#endif

#if defined(UART_SEND_STRING_ASYNC_ENABLE)
	INTERRUPT void UART_TX_COMPLATE_INTERRUPT_VECTOR(void) {
		UART1_ClearITPendingBit(UART1_IT_TC);
		uartTxComplete();	
	}
#endif