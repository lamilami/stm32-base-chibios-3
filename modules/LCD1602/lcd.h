#ifndef LCD1602_H_
#define LCD1602_H_

//---Переопределяем порты для подключения дисплея, для удобства---//
#define     LCM_OUT               GPIOA->ODR
#define     LCM_PIN_RS            GPIOA_PIN10          // PB0
#define     LCM_PIN_EN            GPIOA_PIN9          // PB1
#define     LCM_PIN_D7            GPIOA_PIN3          // PB7
#define     LCM_PIN_D6            GPIOA_PIN2          // PB6
#define     LCM_PIN_D5            GPIOA_PIN1          // PB5
#define     LCM_PIN_D4            GPIOA_PIN0          // PB4

void LCD1602_Start(void *arg);
void InitializeLCD(void);
void ClearLCDScreen(void);
void PrintStr(char *Text);
void LCD_PutSignedInt(int32_t n);
void LCD_PutUnsignedInt(uint32_t n);
void Cursor(char Row, char Col);


#endif
