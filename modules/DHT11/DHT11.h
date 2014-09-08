#ifndef DHT11_H_
#define DHT11_H_

void DHT11_Start();

typedef struct
{
	volatile uint16_t temp;
	volatile uint16_t humidity;
	volatile uint16_t cont_errors;
	union
	{
		volatile uint16_t global_errors[2];
		volatile uint32_t global_errors_32;
	};
	union
	{
		volatile uint16_t critical_errors[2];
		volatile uint32_t critical_errors_32;
	};
} DHT11_Inner_Val;

#endif

