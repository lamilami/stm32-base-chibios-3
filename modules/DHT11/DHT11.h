#ifndef DHT11_H_
#define DHT11_H_

#define DHT11_PIN	GPIOA_PIN0

void DHT11_Start();

typedef struct
{
	volatile systime_t Auto_Update_Sec;
} DHT11_Inner_Val_RW;

typedef struct
{

	DHT11_Inner_Val_RW RW;

	volatile int16_t temp;
	volatile uint16_t humidity;
	volatile uint16_t cont_errors;
	union
	{
		volatile uint16_t global_errors[2];
		volatile uint32_t global_errors_32;
	};

} DHT11_Inner_Val;

#endif

