#ifndef LCD1602_H_
#define LCD1602_H_

//---Переопределяем порты для подключения дисплея, для удобства---//
#define		LCM_PORT			  GPIOA
#define     LCM_OUT               LCM_PORT->ODR
#define     LCM_PIN_RS            PAL_PORT_BIT(GPIOA_PIN5)          // PB0
#define     LCM_PIN_EN            PAL_PORT_BIT(GPIOA_PIN4)          // PB1
#define     LCM_PIN_D7            PAL_PORT_BIT(GPIOA_PIN3)          // PB7
#define     LCM_PIN_D6            PAL_PORT_BIT(GPIOA_PIN2)          // PB6
#define     LCM_PIN_D5            PAL_PORT_BIT(GPIOA_PIN1)          // PB5
#define     LCM_PIN_D4            PAL_PORT_BIT(GPIOA_PIN0)          // PB4

void LCD1602_Start(void *arg);
void InitializeLCD(void);
void ClearLCDScreen(void);
void PrintStr(char *Text);
void LCD_PutSignedInt(int32_t n);
void LCD_PutUnsignedInt(uint32_t n);
void Cursor(char Row, char Col);


#endif
