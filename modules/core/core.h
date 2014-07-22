#ifndef _CORE_H_
#define _CORE_H_

#define MY_ADDR				0x10
#define RADIO_PRESENT		FALSE
#define DS18B20_PRESENT		TRUE
#define FloorHeater_PRESENT TRUE
#define LCD1602_PRESENT 	FALSE
#define WATCHDOG_PRESENT	TRUE


#if (!DS18B20_PRESENT && FloorHeater_PRESENT)
#error "Can't use FloorHeater without DS18B20 temp sensor!"
#endif

#if (LCD1602_PRESENT && (DS18B20_PRESENT || FloorHeater_PRESENT))
#error "LCD1602 conflicts with DS18B20 and FloorHeater!"
#endif

typedef enum
{
	Base, /**< Radio is idle */
	Temp, /**< Maximum number of retries have occured */
	Humidity, /**< Data is sent */
	Light, /**< Data recieved */
	Vent, /**< Ack payload recieved */
	Heater, Other /**< Radio is busy */
} core_types_t;

typedef enum
{
	None, RO, /**< Radio is idle */
	RW
} core_direction_t;

typedef volatile struct core_base_struct core_base_struct_t;

struct core_base_struct
{
	volatile uint8_t id;
	volatile core_types_t type;
//	uint8_t addr;
//	mailbox_t *mbox;
	volatile thread_t *thread;
	volatile core_direction_t direction;
	volatile uint16_t current_value;
	volatile uint16_t set_value;
	volatile void* inner_values;
	volatile uint8_t ival_size;
	volatile const char* description;
	volatile core_base_struct_t *next;
};

extern volatile core_base_struct_t* Core_BasePtr;

void Core_Module_Register(core_base_struct_t* Base_Struct);
uint8_t Core_GetDataById(const uint8_t id, uint16_t* data);
uint16_t Core_SetDataById(const uint8_t id, uint16_t value);

void sleepUntil(systime_t *previous, systime_t period);
void ByteArrayCopy(uint8_t* src, uint8_t* dst, uint8_t cnt);
//#define ABS(a) (((a) < 0) ? -(a) : (a))

void Core_Start(uint8_t id);

#endif
