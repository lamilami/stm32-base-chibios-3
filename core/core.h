#ifndef _CORE_H_
#define _CORE_H_

#define MY_ADDR					0x03

typedef enum
{
	Base, /**< Radio is idle */
	Temp, /**< Maximum number of retries have occured */
	Humidity, /**< Data is sent */
	Light, /**< Data recieved */
	Vent, /**< Ack payload recieved */
	Other /**< Radio is busy */
} core_types_t;

typedef enum
{
	None,
	RO, /**< Radio is idle */
	RW
} core_direction_t;

typedef struct core_base_struct core_base_struct_t;

struct core_base_struct
{
	core_types_t type;
	uint8_t addr;
	mailbox_t *mbox;
	core_direction_t direction;
	const char *description;
	core_base_struct_t *next;

/*	union
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
		uint8_t pload[RF_MAX_PAYLOAD_LENGTH+2];
		struct
		{
			uint8_t src_addr;
			uint8_t dst_addr;
			uint8_t cmd;
			uint8_t data[RF_MAX_PAYLOAD_LENGTH-1];
		};
	};*/
};

core_base_struct_t Core_Base;

#endif
