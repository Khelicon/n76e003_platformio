/*
 * N76E003 GPIOs toggle.
 *
 * arad.rgb@gmail.com
 */
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"

#define UART_BAUDRATE	(115200)

uint8_t uart_data = 0x56;
bit_t uart_ri_flag;

void SerialPort0_ISR(void) __interrupt 4
{
	if (RI) {					// uart rx done
		clr_RI;					// clear rx flag
		uart_data = SBUF;
		uart_ri_flag = 1;
	}
	if(TI) {					// uart tx done
		clr_TI;					// clear tx flag
	}
}

#define RELOAD_VALUE_H  (65536-1500)/256
#define RELOAD_VALUE_L  (65536-1500)%256

/************************************************************************************************************
*    TIMER 2 interrupt subroutine
************************************************************************************************************/
void Timer3_ISR (void) __interrupt 16 
{
    clr_TF3;
    P00 = ~P00;                                 //P0.2 toggle when TF3 interrupt
     //printf("\n TM3 INT 0x%bX", RH3);
}

void main(void)
{
	Set_All_GPIO_Quasi_Mode;	// Reset all GPIOs to inputs

	//P0M1 = P1M1 = 0x00;						// set P0.0-P0.7 and P1.0-P1.7 GPIOs to PushPull
	//P0M2 = P1M2 = 0xff;
	//P30_PushPull_Mode;						// set P3.0 to PushPull
	
	
	InitialUART0_Timer3(UART_BAUDRATE);
	set_ES;										// enable uart interrupt
	
	RH3 = RELOAD_VALUE_H;                       //initial counter values 
    RL3 = RELOAD_VALUE_L;    
    
    set_ET3;                                    //enable Timer3 interrupt
    set_EA;                                     //enable interrupts
    set_TR3;                                    //Timer3 run


	while(1) {					// Toggle GPIOs every 100ms
	  set_P10;
      clr_P01;
      clr_P11;
		Timer0_Delay1ms(250); 
      clr_P10;
      set_P01;
      set_P11;
		Timer0_Delay1ms(100);

		Send_Data_To_UART0(uart_data);
	}
}
