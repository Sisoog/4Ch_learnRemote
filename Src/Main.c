/*
Main.c
Created on: May 9, 2017
Copyright (C) Mohammad Mazarei Zeus @ Sisoog.com
Email : mohammad.mazarei@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <EV17xx_Decoder.h>
#include <avr/interrupt.h>
#include <eeprom_store.h>
#include <util/delay.h>
#include <avr/io.h>
#include <uart.h>

#define Key_Pin		3
#define Key_PORT	PORTD
#define Key_DDR		DDRD
#define Key_PIN		PIND

#define LED_Pin		4
#define	LED_PORT	PORTC
#define LED_DDR		DDRC

#define CH0_Pin		0
#define CH1_Pin		1
#define CH2_Pin		2
#define CH3_Pin		3
#define CH_PORT		PORTC
#define CH_DDR		DDRC

#define IS_Press_Key()		((Key_PIN&_BV(Key_Pin))==0)
#define LED_Tog()			LED_PORT ^= _BV(LED_Pin)
#define LED_On()			LED_PORT |= _BV(LED_Pin)

enum
{
	Nurmal = 0,
	Learn,
	Erase,
};



int main(void)
{
	uart_init();
	Key_DDR &=~_BV(Key_Pin); /*PORTD.3 As Input*/
	Key_PORT |=_BV(Key_Pin); /*PORTD.3 Enable Pullup*/
	LED_DDR |= _BV(LED_Pin);
	CH_DDR |= (_BV(CH0_Pin) | _BV(CH1_Pin) | _BV(CH2_Pin) | _BV(CH3_Pin));


	// External Interrupt(s) initialization
	// INT0: On
	// INT0 Mode: Any change
	EICRA=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (1<<ISC00);
	EIMSK=(0<<INT1) | (1<<INT0);
	EIFR=(0<<INTF1) | (1<<INTF0);


	sei();

	printf("Zeus 4CH Remote Control\n");
	printf("This Code Write By Mohammad Mazarei :)\n");
	uint32_t Press_Time = 0;
	uint32_t SysTime = 0;
	uint32_t LEDTime = 0;
	uint8_t  Work_Mode = Nurmal;
	uint32_t Lcode = 0;
	uint32_t CodeTime = 0;
	while(1)
	{

		if(IS_Recive_Valid_Remote()) /*If Have Code For Remote*/
		{
			uint32_t Rcode = Get_Remote_Code_long();
			CodeTime = SysTime;
			if(Lcode!=Rcode)
			{
				Lcode = Rcode;

				if(Work_Mode==Nurmal)
				{
					if(EE_ISValidCode(Rcode&0xFFFFFFF0)) /*If Valid Code*/
					{
						uint8_t Rkey = Rcode&0xFF;
						printf("Code Is Valid Action...%x\n",Rkey);

						if(Rkey&1)
							CH_PORT ^= _BV(CH0_Pin);
						if(Rkey&2)
							CH_PORT ^= _BV(CH1_Pin);
						if(Rkey&4)
							CH_PORT ^= _BV(CH2_Pin);
						if(Rkey&8)
							CH_PORT ^= _BV(CH3_Pin);
					}
					else
					{
						printf("This Remote Not Valid!\n");
					}
				}
				else if(Work_Mode==Learn)
				{
					if(!EE_ISValidCode(Rcode&0xFFFFFFF0)) /*If Not Have Code*/
					{
						printf("Learn This Remote...\n");
						EE_StoreCode(Rcode&0xFFFFFFF0);
					}
					Work_Mode = Nurmal;
				}
			}
		}
		else
		{
			if((SysTime - CodeTime) > 500)
			{
				Lcode = 0; /*Reset Last Code*/
			}
		}

		if(IS_Press_Key())
		{
			Press_Time++;
		}
		else
		{
			if(Press_Time>1000 && Press_Time<3000)
			{
				printf("Enter Learn Mode\n");
				Work_Mode = Learn;
			}
			else if(Press_Time>5000 && Press_Time<15000)
			{
				printf("Enter Erase Mode\n");
				LED_On();
				EE_Format();
				_delay_ms(3000);
				LED_Tog();
			}
			Press_Time = 0;
		}

		/*Blink Control LED*/
		if(Work_Mode==Nurmal)
		{
			if(LEDTime==0 || (SysTime-LEDTime) > 1000) /*Evry 1Sec*/
			{
				LEDTime = SysTime;
				LED_Tog();
			}
		}
		else if(Work_Mode==Learn)
		{
			if(LEDTime==0 || (SysTime-LEDTime) > 100) /*Evry 100 mili Sec*/
			{
				LEDTime = SysTime;
				LED_Tog();
			}
		}

		SysTime++;
		_delay_ms(1);
	}
	return 0;
}
