#ifndef MPC_H_
#define MPC_H_


#if MPC_PRESENT
thread_t *MPC_Thread;

#define EVENTMASK_SEND 0x1
#define EVENTMASK_RADIO_IRQ 0x2
#define EVENTMASK_UART_IRQ 0x4
#define EVENTMASK_UART_TX_IRQ 0x8

/** Defines the payload length the radio should use */
#define MPC_MAX_PAYLOAD_LENGTH 32

typedef enum
{
//  команды
    RF_NOP = 0x00,
    RF_PING,
    RF_PONG,
    RF_GET,
    RF_PUT,
    RF_SET,
    RF_OK,
    RF_FAIL,
    RF_END = 0xFF
} MPC_commands_t;

void MPC_Start(void);
uint8_t MPC_Send_Command(uint8_t rcv_addr, MPC_commands_t command, uint8_t data_size, void *data);

typedef struct
{
    union
    {
        uint16_t whole_size_pipenum;
        struct
        {
            uint8_t size;
            uint8_t pipenum;
        };
    };
    union
    {
        uint8_t pload[MPC_MAX_PAYLOAD_LENGTH + 2];
        struct
        {
            uint8_t src_addr;
            uint8_t dst_addr;
            uint8_t cmd;
            uint8_t data[MPC_MAX_PAYLOAD_LENGTH - 1];
        };
    };
} payload_t;

#endif

#endif
