/*
 * Mbed OS 5 Test01
 *
 * 2018.09.25
 *
 */

#include "mbed.h"
#include "u8g2.h"

#define TITLE_STR1  ("Mbed OS 5 Test01")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

uint8_t u8x8_gpio_and_delay_mbed(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
u8g2_t myScreen;

AnalogIn Adc1(A0);
DigitalOut Led1(LED1);

Thread thDisplay;
Thread thAdc;

volatile float v1;

//-------------------------------------------------------------------------------------------------------------
// Thread Callback
//

void readAdc()
{
	v1 = Adc1.read();
}

void display()
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&myScreen);
	u8g2_SetFont(&myScreen, u8g2_font_10x20_mf);
	sprintf(strBuffer, "v1: %f", v1); 
	u8g2_DrawStr(&myScreen, 0, 16, strBuffer);
	u8g2_SendBuffer(&myScreen);
}

//-------------------------------------------------------------------------------------------------------------
// Main Routine
//
void u8g2Initialize()
{
	u8g2_Setup_ssd1306_i2c_128x32_univision_f(&myScreen, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_mbed);
	u8g2_InitDisplay(&myScreen);
	u8g2_SetPowerSave(&myScreen, 0);
	u8g2_ClearBuffer(&myScreen);

	u8g2_SetFont(&myScreen, u8g2_font_10x20_mf);
	u8g2_DrawStr(&myScreen, 0, 16, TITLE_STR1);
	u8g2_DrawStr(&myScreen, 0, 32, TITLE_STR2);
	u8g2_SendBuffer(&myScreen);
}

int main()
{
	u8g2Initialize();
	wait(3.0);
	
	while(1) {
		
		readAdc();
		display();
		
		if (v1 > 0.5) {
			Led1.write(1);
		}
		else {
			Led1.write(0);
		}
	}
}
