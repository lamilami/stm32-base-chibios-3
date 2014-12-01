#include "ch.h"
#include "hal.h"
#include "lcd.h"
#include "core.h"

#if LCD1602_PRESENT

#define     LCM_PIN_MASK  ((LCM_PIN_RS | LCM_PIN_EN | LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5 | LCM_PIN_D4))
//GPIO_InitTypeDef  GPIO_InitStructure;

/*
 //---Функция задержки---//
 void delay(int a)
 {
 int i = 0;
 int f = 0;
 while(f<a)
 {
 while(i<60)
 {i++;}
 f++;
 }
 }
 */

#define delay chThdSleepMicroseconds

//---Нужная функция для работы с дисплее, по сути дергаем ножкой EN---//
void PulseLCD() {
	LCM_OUT &= ~(LCM_PIN_EN);
	delay(1);
	LCM_OUT |= (LCM_PIN_EN);
	delay(1);
	LCM_OUT &= (~(LCM_PIN_EN));
	delay(1);
}

//---Отсылка байта в дисплей---//
void LCD_SendByte(char ByteToSend, int IsData) {
	LCM_OUT &= (~(LCM_PIN_MASK));
	LCM_OUT |= ((ByteToSend & 0xF0) >> 4);

	if (IsData == 1)
		LCM_OUT |= (LCM_PIN_RS);
	else
		LCM_OUT &= ~(LCM_PIN_RS);
	PulseLCD();
	LCM_OUT &= (~(LCM_PIN_MASK));
	LCM_OUT |= (ByteToSend & 0x0F);

	if (IsData == 1)
		LCM_OUT |= (LCM_PIN_RS);
	else
		LCM_OUT &= ~(LCM_PIN_RS);

	PulseLCD();
}

//---Установка позиции курсора---//
void Cursor(char Row, char Col) {
	char address;
	if (Row == 0)
		address = 0;
	else
		address = 0x40;
	address |= Col;
	LCD_SendByte(0x80 | address, 0);
}

//---Очистка дисплея---//
void ClearLCDScreen() {
	LCD_SendByte(0x01, 0);
	LCD_SendByte(0x02, 0);
}

//---Инициализация дисплея---//
void InitializeLCD(void) {
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	rccEnableAHB(RCC_AHBENR_GPIOAEN, TRUE);
	/*
	 GPIO_InitStructure.GPIO_Pin = LCM_PIN_RS | LCM_PIN_EN | LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5 | LCM_PIN_D4;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_Init(((GPIO_TypeDef *) GPIOA_BASE), &GPIO_InitStructure);
	 */
	palSetGroupMode(GPIOA,
			LCM_PIN_RS | LCM_PIN_EN | LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5
					| LCM_PIN_D4, 0, PAL_MODE_OUTPUT_PUSHPULL);
//	palSetPadMode(GPIOA, LCM_PIN_RS,PAL_MODE_OUTPUT_PUSHPULL);
//	palSetPadMode(GPIOA, LCM_PIN_EN,PAL_MODE_OUTPUT_PUSHPULL);
//	palSetPadMode(GPIOA, LCM_PIN_D7,PAL_MODE_OUTPUT_PUSHPULL);
//	palSetPadMode(GPIOA, LCM_PIN_D6,PAL_MODE_OUTPUT_PUSHPULL);
//	palSetPadMode(GPIOA, LCM_PIN_D5,PAL_MODE_OUTPUT_PUSHPULL);
//	palSetPadMode(GPIOA, LCM_PIN_D4,PAL_MODE_OUTPUT_PUSHPULL);

	LCM_OUT &= ~((LCM_PIN_MASK));
	chThdSleepMilliseconds(15);
	LCM_OUT &= ~(LCM_PIN_RS);
	LCM_OUT &= ~(LCM_PIN_EN);
	LCM_OUT = 0x03;
	PulseLCD();
	chThdSleepMilliseconds(5);
	PulseLCD();
	chThdSleepMicroseconds(100);
	PulseLCD();
	chThdSleepMicroseconds(50);
	LCM_OUT = 0x02;
	PulseLCD();
	chThdSleepMicroseconds(50);

	LCD_SendByte(0x28, 0);
	LCD_SendByte(0x0E, 0);
	LCD_SendByte(0x06, 0);
}

//---Печать строки---//
void PrintStr(char *Text) {
	char *c;
	c = Text;
	while ((c != 0) && (*c != 0)) {
		LCD_SendByte(*c, 1);
		c++;
	}
}

void LCD_PutSignedInt(int32_t n) {
	char c1[32];

	if (n == 0) {
		LCD_SendByte('0', 1);
		return;
	}

	signed int value = n;
	unsigned int absolute;

	int i = 0;

	if (value < 0) {
		absolute = -value;
	} else {
		absolute = value;
	}

	while (absolute > 0) {
		c1[i] = '0' + absolute % 10;
		absolute /= 10;
		i++;
	}

	LCD_SendByte((value < 0) ? '-' : '+', 1);

	i--;

	while (i >= 0) {
		LCD_SendByte(c1[i--], 1);
	}
}

void LCD_PutUnsignedInt(uint32_t n) {
	char c1[32];
	uint32_t value = n;
	uint32_t i = 0;

	if (n == 0) {
		LCD_SendByte('0', 1);
		return;
	}

	while (value > 0) {
		c1[i] = '0' + value % 10;
		value /= 10;
		i++;
	}

	while (i-- > 0) {
		LCD_SendByte(c1[i], 1);
	}
}

void LCD_PutSignedQ2(int32_t n) {
//	char c1[32];

	if (n == 0) {
		LCD_SendByte('0', 1);
		return;
	}

//	signed int value = n;
	unsigned int absolute;

//	int i = 0;

	if (n < 0) {
		absolute = -n;
	} else {
		absolute = n;
	}

	unsigned int fract;

	fract = (absolute & 3)*25;
	absolute = absolute >> 2;

/*	if (fract == 0) {
		c1[i] = '0';
		i++;
	} else {
		while (fract > 0) {
			c1[i] = '0' + fract % 10;
			fract /= 10;
			i++;
		}
	}

	c1[i] = '.';

	if (absolute == 0) {
		i++;
		c1[i] = '0';
	} else {
		while (absolute > 0) {
			i++;
			c1[i] = '0' + absolute % 10;
			absolute /= 10;
		}
	}

	LCD_SendByte((value < 0) ? '-' : '+', 1);

	while (i >= 0) {
		LCD_SendByte(c1[i--], 1);
	}*/

	LCD_SendByte((n < 0) ? '-' : '+', 1);
	LCD_PutUnsignedInt(absolute);
	LCD_SendByte('.', 1);
	LCD_PutUnsignedInt(fract);
}

#endif
