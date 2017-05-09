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
#ifndef _UARTCONF
#define _UARTCONF
#include <stdio.h>

// Use uscanf and uprintf function
#ifndef F_CPU
#define F_CPU 4000000UL
#endif



/* UART baud rate */
#define UART_BAUD  9600

//#define USE_INLINE_EDITOR
#define RX_BUFSIZE 10



void	uart_init(void);
int	uart_putchar(char c, FILE *stream);
int	uart_getchar(FILE *stream);



extern FILE uart_str;


// uprintf macro , fprintf with UART Stream
#define uprintf(args...) fprintf(&uart_str,args)
#define uscanf(args...) fscanf(&uart_str,args)

#endif
