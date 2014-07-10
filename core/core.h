#ifndef _CORE_H_
#define _CORE_H_

#define MY_ADDR			0x03


typedef enum
{
	Base, /**< Radio is idle */
	Temp, /**< Maximum number of retries have occured */
	Humidity, /**< Data is sent */
	Light, /**< Data recieved */
	Vent, /**< Ack payload recieved */
	Heater,
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
	uint8_t				id;
	core_types_t 		type;
//	uint8_t addr;
//	mailbox_t *mbox;
	thread_t 			*thread;
	core_direction_t 	direction;
	uint16_t			current_value;
	uint16_t			set_value;
	void*				inner_values;
	uint8_t				ival_size;
	const char*			description;
	core_base_struct_t *next;
};

core_base_struct_t Core_Base;

void Core_Module_Register (core_base_struct_t Base_Struct);
uint8_t Core_GetDataById(const uint8_t id, const uint8_t val, uint16_t* data);

void sleepUntil(systime_t *previous, systime_t period);

#endif
