#ifndef DHT11_H_
#define DHT11_H_

#define DHT11_QUANTITY 1

#define DHT11_0_PIN	        GPIOA_PIN0
#define DHT11_0_PORT        GPIOA
#define DHT11_0_EXT_PORT    EXT_MODE_GPIOA
#define DHT11_1_PIN         GPIOA_PIN3
#define DHT11_1_PORT        GPIOA
#define DHT11_1_EXT_PORT    EXT_MODE_GPIOA

void DHT11_Start();

typedef struct
{
	volatile systime_t Auto_Update_Sec;
} DHT11_Inner_Val_RW;

typedef struct
{

	DHT11_Inner_Val_RW RW;

	volatile int16_t temp;
	volatile int16_t humidity;
	volatile uint16_t cont_errors;
//  union {
	volatile uint16_t global_errors;
//    volatile uint32_t global_errors_32;
//  };

} DHT11_Inner_Val;

#endif

