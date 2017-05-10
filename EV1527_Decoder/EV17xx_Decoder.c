/*
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

#include "EV17xx_Decoder.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "avr/io.h"
#include "Timer.h"



#define IS_Sync_Start_Pulse(T1,T2)		(T2 > (T1*29) && T2 < (T1*32))
#define Bit_IS_Zero(T1,T2)				(T2 > (T1*2)  && T2 < (T1*4))
#define Bit_IS_One(T1,T2)				(T1 > (T2*2)  && T1 < (T2*4))

volatile uint32_t    EV_Code = 0;
volatile uint8_t	 Revice_Flag = 0;


const uint32_t Bit_Shift[32] PROGMEM =
{
	0x00000001,0x00000002,0x00000004,0x00000008,
	0x00000010,0x00000020,0x00000040,0x00000080,
	0x00000100,0x00000200,0x00000400,0x00000800,
	0x00001000,0x00002000,0x00004000,0x00008000,
	0x00010000,0x00020000,0x00040000,0x00080000,
	0x00100000,0x00200000,0x00400000,0x00800000,
	0x01000000,0x02000000,0x04000000,0x08000000,
	0x10000000,0x20000000,0x40000000,0x80000000,
};

ISR(INT0_vect)	// interrupt Of Micro
{
	static uint16_t	Time_Falling=0;
	static uint16_t	Time_Rising=0;
	static uint16_t	Start_Sync = 0;
	static uint8_t	Bit_Index = 0;
	static uint32_t Receive_Code = 0;

	if(PIND&(1<<2)) // Rising Edge
	{

		Time_Falling =Stop_Read_timer();


		if(IS_Sync_Start_Pulse(Time_Rising,Time_Falling))	/* Start Sync*/
		{
			Start_Sync = 1;
			Bit_Index = 0;
			Receive_Code = 0;
		}
		else
		{
			if(Start_Sync==1)	// Start Sended
			{
				if(Bit_Index < 23)
				{
					if(Bit_IS_Zero(Time_Rising,Time_Falling))
					{
						Bit_Index++;
					}
					else if(Bit_IS_One(Time_Rising,Time_Falling))
					{
						Receive_Code |= pgm_read_dword(&Bit_Shift[(23-Bit_Index)]);
						Bit_Index++;
					}
					else
					{
						Start_Sync = 0;
						Bit_Index = 0;
					}

				}
				else
				{	// All Bit Recive
					Bit_Index = 0;
					Start_Sync = 0;
					EV_Code = Receive_Code;
					Revice_Flag = 1;
				}
			}	// End of Start Sync Send
		}

		Start_Timer();
	}
	else	//Falling Edge
	{

		Time_Rising = Stop_Read_timer() ;
		Start_Timer();
	}
}


uint8_t	IS_Recive_Valid_Remote(void)
{
	return (Revice_Flag==1);
}

void Get_Remote_Code(uint8_t *Code)
{
	Code[0] = EV_Code&0xFF;
	Code[1] = (EV_Code>>8)&0xFF;
	Code[2] = (EV_Code>>16)&0xFF;
	Revice_Flag = 0;
}

uint32_t Get_Remote_Code_long(void)
{

	Revice_Flag = 0;
	return EV_Code;
}
