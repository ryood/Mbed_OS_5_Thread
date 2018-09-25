/*
 * Mbed OS 5 Thread Test04
 *
 * 2018.09.25
 *
 */

#include "mbed.h"
#include "u8g2.h"

#define SET_PRIORITY (1)

#define TITLE_STR1  ("Thread Test04")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define SAMPLING_RATE (50000.0f)  // Hz
#define PI_F          (3.1415926f)

uint8_t u8x8_gpio_and_delay_mbed(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
u8g2_t myScreen;

AnalogIn Adc1(A0);
AnalogIn Adc2(A1);
AnalogIn Adc3(A2);
AnalogIn Adc4(A3);

AnalogOut Dac1(A2);

DigitalOut Led1(LED1);

DigitalOut CheckPin1(D2);
DigitalOut CheckPin2(D3);
DigitalOut CheckPin3(D4);

Ticker dacHandler;

Thread thDisplay;
Thread thAdc;

volatile float v1;
volatile float v2;
volatile float v3;
volatile float v4;

volatile float phase = 0.0f;

//-------------------------------------------------------------------------------------------------------------
// Ticker
//
void writeDac()
{
	CheckPin1.write(1);

	phase += 1.0f / (SAMPLING_RATE / (v1 * 900.0f + 100.0f));  // 100Hz ~ 1000Hz
	if (phase > 1.0f) {
		phase = 0.0f;
	}
	
	float rad = 2.0f * PI_F * phase;
	float amp = (sinf(rad) * v2) / 2.0f + 0.5f;
	
	Dac1.write(amp);
			
	CheckPin1.write(0);
}

//-----------------------------------------------------------------------------
// Thread Callback
//

void readAdc()
{
	while (1) {
		CheckPin3.write(1);

		v1 = Adc1.read();
		v2 = Adc2.read();
		v3 = Adc3.read();
		v4 = Adc4.read();

		CheckPin3.write(0);
	}
}

void display()
{
	char strBuffer[20];
	
	while(1) {
		CheckPin2.write(1);

		u8g2_ClearBuffer(&myScreen);
		u8g2_SetFont(&myScreen, u8g2_font_10x20_mf);
		sprintf(strBuffer, "%6.4f %6.4f", v1, v2); 
		u8g2_DrawStr(&myScreen, 0, 16, strBuffer);
		sprintf(strBuffer, "%6.4f %6.4f", v3, v4); 
		u8g2_DrawStr(&myScreen, 0, 32, strBuffer);
		u8g2_SendBuffer(&myScreen);

		CheckPin2.write(0);
	}
}

//-----------------------------------------------------------------------------
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

#if (SET_PRIORITY)
	thAdc.set_priority(osPriorityHigh);
	thDisplay.set_priority(osPriorityLow);
#endif //SET_PRIORITY
	
	thAdc.start(readAdc);
	thDisplay.start(display);
	
	float period = 1.0f / SAMPLING_RATE;
	dacHandler.attach(&writeDac, period);
	
	while(1) {
		CheckPin1.write(1);
		if (v1 > 0.5) {
			Led1.write(1);
		}
		else {
			Led1.write(0);
		}
		CheckPin1.write(0);
		wait(0.1);
	}
}
