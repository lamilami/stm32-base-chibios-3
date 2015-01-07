/**
 ******************************************************************************
 * @file      semihosting.h
 * @author    Coocox
 * @version   V1.0
 * @date      09/10/2011
 * @brief     Semihosting Low Layer GetChar/SendChar API.
 *
 *******************************************************************************
 */
#ifndef __SIMIHOSTTING_IMPL
#define __SIMIHOSTTING_IMPL

/********************************************************************************************************//**
 * Semihosting functions prototype
************************************************************************************************************/
extern int SH_DoCommand(int n32In_R0, int n32In_R1, int *pn32Out_R0);
void SH_SendChar(int ch);
void SH_SendString(const char *str);
char SH_GetChar(void);

void SH_PrintStr(char *Text);
void SH_PutSignedInt(int32_t n);
void SH_PutUnsignedInt(uint32_t n);
void SH_PutSignedQ2(int32_t n);

#endif


