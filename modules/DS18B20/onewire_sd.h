/*
 * onewire.h
 *
 *  Version 1.0.1
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

// первый параметр функции OW_Send
#define OW_SEND_RESET		1
#define OW_NO_RESET		2

// статус возврата функций
#define OW_OK			1
#define OW_ERROR		2
#define OW_NO_DEVICE	3

#define OW_NO_READ		0xff

#define OW_READ_SLOT	0xff

uint8_t OW_Init();
uint8_t OW_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen,
		uint8_t *data, uint8_t dLen, uint8_t readStart);
uint8_t OW_Scan(uint8_t *buf, uint8_t num);

#define OW_SEARCH_ROM		(uint8_t*)"\xf0"

// shortcuts for functions
// only send message b wich length is c with RESET flag a
#define OW_SendOnly(a,b,c)  OW_Send(a, b, c, (void*)0, 0, OW_NO_READ)
// send 1 command (with bus reset)
#define OW_WriteCmd(cmd) OW_Send(1, cmd, 1, (void*)0, 0, OW_NO_READ)
// send 1 function (without bus reset)
#define OW_WriteFn(cmd) OW_Send(0, cmd, 1, (void*)0, 0, OW_NO_READ)

#endif /* ONEWIRE_H_ */
