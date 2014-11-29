#ifndef DS18B20_H_
#define DS18B20_H_

#define DS18B20_NUMBER_OF_SENSORS	1

/*
#define DS18B20_SENSOR_1			"\x55\x28\xbb\x58\xbf\x03\x00\x00\xa5\xbe\xff\xff"
*/

//#define DS18B20_SENSOR_1			"\x55\x28\x08\x38\xbf\x03\x00\x00\xea\xbe\xff\xff"
//#define DS18B20_SENSOR_2			"\x55\x28\xd8\x2a\xbf\x03\x00\x00\xd1\xbe\xff\xff"
//#define DS18B20_SENSOR_3			"\x55\x28\x0b\x51\xbf\x03\x00\x00\x51\xbe\xff\xff"
//#define DS18B20_SENSOR_4			"\x55\x28\x97\xbb\xd5\x03\x00\x00\x71\xbe\xff\xff"

//#define DS18B20_SENSOR_5			"\x55\x28\xff\xf9\x5e\x4d\x04\x00\xca\xbe\xff\xff"

//#define DS18B20_SENSOR_1			"\x55\x28\x8b\x2d\xbf\x03\x00\x00\xf7\xbe\xff\xff"
#define DS18B20_SENSOR_1			"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

void DS18B20_Start();

typedef struct
{
	volatile systime_t Auto_Update_Sec;
} DS18B20_Inner_Val_RW;

typedef struct
{

	DS18B20_Inner_Val_RW RW;

	volatile int16_t temp[DS18B20_NUMBER_OF_SENSORS];
//	const uint8_t* sens_addr;// = {DS18B20_SENSOR_1,DS18B20_SENSOR_2,DS18B20_SENSOR_3,DS18B20_SENSOR_4};
	volatile uint16_t cont_errors;
	union
	{
		volatile uint16_t global_errors[2];
		volatile uint32_t global_errors_32;
	};
/*	union
	{
		volatile uint16_t critical_errors[2];
		volatile uint32_t critical_errors_32;
	};*/

} DS18B20_Inner_Val;


#endif
