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


#include <avr/io.h>
#include <uart.h>
#include <EV17xx_Decoder.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	uart_init();
	DDRD |=(1<<3);
	// External Interrupt(s) initialization
	// INT0: On
	// INT0 Mode: Any change
	// INT1: Off
	// Interrupt on any change on pins PCINT0-7: Off
	// Interrupt on any change on pins PCINT8-14: Off
	// Interrupt on any change on pins PCINT16-23: Off
	EICRA=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (1<<ISC00);
	EIMSK=(0<<INT1) | (1<<INT0);
	EIFR=(0<<INTF1) | (1<<INTF0);
	PCICR=(0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);

	sei();



	uint8_t Code[4];
	printf("Abas Salam ");
	while(1)
	{

		if(IS_Recive_Valid_Remote())
		{
			Get_Remote_Code(Code);
			printf("---%X-%X-%X\r",Code[2],Code[1],Code[0]);
		}
				_delay_ms(10);
	}
	return 0;
}
