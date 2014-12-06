/**
 ******************************************************************************
 * @file      semihosting.c
 * @author    Coocox
 * @version   V1.0
 * @date      09/10/2011
 * @brief     Semihosting LowLayer GetChar/SendChar Implement.
 *
 *******************************************************************************
 */
 
#include <sys/types.h>
#include <sys/stat.h>
#include "ch.h"
#include "hal.h"
#include "semihosting.h"

static char g_buf[16];
static char g_buf_len = 0;

/**************************************************************************//**
 * @brief  Transmit a char on semihosting mode.
 *
 * @param  ch is the char that to send.
 *
 * @return Character to write.
 *****************************************************************************/
void SH_SendChar(int ch) {
	g_buf[g_buf_len++] = ch;
	g_buf[g_buf_len] = '\0';
	if (g_buf_len + 1 >= sizeof(g_buf) || ch == '\n' || ch == '\0') {
		g_buf_len = 0;
		/* Send the char */
		if (SH_DoCommand(0x04, (int) g_buf, NULL) != 0) {
			return;
		}
	}
}

/**************************************************************************//**
 * @brief  Transmit a null-terminated string on semihosting mode.
 *
 * @param  str is the string that to send.
 *
 * @return Character to write.
 *****************************************************************************/
void SH_SendString(const char *str)
{
//	int j;
	if (SH_DoCommand(0x04, (int)str, NULL) != 0) {
		return;
	}
}

/**************************************************************************//**
 * @brief  Read a char on semihosting mode.
 *
 * @param  None.
 *
 * @return Character that have read.
 *****************************************************************************/
char SH_GetChar() {
	int nRet;

	while (SH_DoCommand(0x101, 0, &nRet) != 0) {
		if (nRet != 0) {
			SH_DoCommand(0x07, 0, &nRet);
			return (char) nRet;
		}
	}

	return 0;
}

//---Печать строки---//
void SH_PrintStr(char *Text) {
	char *c;
	c = Text;
	while ((c != 0) && (*c != 0)) {
		SH_SendChar(*c);
		c++;
	}
}

void SH_PutSignedInt(int32_t n) {
	char c1[32];

	if (n == 0) {
		SH_SendChar('0');
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

	SH_SendChar((value < 0) ? '-' : '+');

	i--;

	while (i >= 0) {
		SH_SendChar(c1[i--]);
	}
}

void SH_PutUnsignedInt(uint32_t n) {
	char c1[32];
	uint32_t value = n;
	uint32_t i = 0;

	if (n == 0) {
		SH_SendChar('0');
		return;
	}

	while (value > 0) {
		c1[i] = '0' + value % 10;
		value /= 10;
		i++;
	}

	while (i-- > 0) {
		SH_SendChar(c1[i]);
	}
}

void SH_PutSignedQ2(int32_t n) {
//	char c1[32];

	if (n == 0) {
		SH_SendChar('0');
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

	SH_SendChar((n < 0) ? '-' : '+');
	SH_PutUnsignedInt(absolute);
	SH_SendChar('.');
	SH_PutUnsignedInt(fract);
}
