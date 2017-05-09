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


#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"

#if defined (__AVR_ATmega328P__)

#define CHIP_UCSRA		UCSR0A
#define CHIP_UCSRB		UCSR0B
#define CHIP_UBRRL		UBRR0L
#define CHIP_UDR		UDR0
#define CHIP_RXEN		RXC0
#define CHIP_TXEN		TXEN0
#define CHIP_UDRE		UDRE0
#define CHIP_DOR		DOR0
#define CHIP_FE			FE0
#define CHIP_RXC		RXC0

#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega32A__) || defined (__AVR_ATmega16__) || defined (__AVR_ATmega16A__)

#define CHIP_UCSRA		UCSRA
#define CHIP_UCSRB		UCSRB
#define CHIP_UBRRL		UBRRL
#define CHIP_UDR		UDR
#define CHIP_RXEN		RXC
#define CHIP_TXEN		TXEN
#define CHIP_UDRE		UDRE
#define CHIP_DOR		DOR
#define CHIP_FE			FE
#define CHIP_RXC		RXC

#else
	#error "This Library Not Support This CPU"
#endif



FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void uart_init(void)
{
#if F_CPU < 2000000UL && defined(U2X)
	CHIP_UCSRA = _BV(U2X);             /* improve baud rate error by using 2x clk */
	CHIP_UBRRL = (F_CPU / (8UL * UART_BAUD)) - 1;
#else
	CHIP_UBRRL = (F_CPU / (16UL * UART_BAUD)) - 1;
#endif
	CHIP_UCSRB = _BV(CHIP_TXEN) | _BV(CHIP_RXEN); /* tx/rx enable */
  

  stdout = stdin = &uart_str;
}

/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int
uart_putchar(char c, FILE *stream)
{

  if (c == '\a')
    {
      return 0;
    }

  if (c == '\n')
    uart_putchar('\r', stream);
  loop_until_bit_is_set(CHIP_UCSRA, CHIP_UDRE);
  CHIP_UDR = c;
  return 0;
}

/*
 * Receive a character from the UART Rx.
 *
 * This features a simple line-editor that allows to delete and
 * re-edit the characters entered, until either CR or NL is entered.
 * Printable characters entered will be echoed using uart_putchar().
 *
 * Editing characters:
 *
 * . \b (BS) or \177 (DEL) delete the previous character
 * . ^u kills the entire input buffer
 * . ^w deletes the previous word
 * . ^r sends a CR, and then reprints the buffer
 * . \t will be replaced by a single space
 *
 * All other control characters will be ignored.
 *
 * The internal line buffer is RX_BUFSIZE (80) characters long, which
 * includes the terminating \n (but no terminating \0).  If the buffer
 * is full (i. e., at RX_BUFSIZE-1 characters in order to keep space for
 * the trailing \n), any further input attempts will send a \a to
 * uart_putchar() (BEL character), although line editing is still
 * allowed.
 *
 * Input errors while talking to the UART will cause an immediate
 * return of -1 (error indication).  Notably, this will be caused by a
 * framing error (e. g. serial line "break" condition), by an input
 * overrun, and by a parity error (if parity was enabled and automatic
 * parity recognition is supported by hardware).
 *
 * Successive calls to uart_getchar() will be satisfied from the
 * internal buffer until that buffer is emptied again.
 */
int
uart_getchar(FILE *stream)
{
  uint8_t c;
 

#ifdef USE_INLINE_EDITOR
static char *rxp;
char *cp, *cp2;
static char b[RX_BUFSIZE];
  
  if (rxp == 0)
    for (cp = b;;)
      {
	loop_until_bit_is_set(UCSRA, RXC);
	if (UCSRA & _BV(FE))
	  return _FDEV_EOF;
	if (UCSRA & _BV(DOR))
	  return _FDEV_ERR;
	c = UDR;
	
	
	if (c == '\r')
	  c = '\n';
	if (c == '\n')
	  {
	    *cp = c;
	    uart_putchar(c, stream);
	    rxp = b;
	    break;
	  }
	else if (c == '\t')
	  c = ' ';

	if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') ||
	    c >= (uint8_t)'\xa0')
	  {
	    if (cp == b + RX_BUFSIZE - 1)
	      uart_putchar('\a', stream);
	    else
	      {
		*cp++ = c;
		uart_putchar(c, stream);
	      }
	    continue;
	  }

	switch (c)
	  {
	  case 'c' & 0x1f:
	    return -1;

	  case '\b':
	  case '\x7f':
	    if (cp > b)
	      {
		uart_putchar('\b', stream);
		uart_putchar(' ', stream);
		uart_putchar('\b', stream);
		cp--;
	      }
	    break;

	  case 'r' & 0x1f:
	    uart_putchar('\r', stream);
	    for (cp2 = b; cp2 < cp; cp2++)
	      uart_putchar(*cp2, stream);
	    break;

	  case 'u' & 0x1f:
	    while (cp > b)
	      {
		uart_putchar('\b', stream);
		uart_putchar(' ', stream);
		uart_putchar('\b', stream);
		cp--;
	      }
	    break;

	  case 'w' & 0x1f:
	    while (cp > b && cp[-1] != ' ')
	      {
		uart_putchar('\b', stream);
		uart_putchar(' ', stream);
		uart_putchar('\b', stream);
		cp--;
	      }
	    break;
	  }
      }

  c = *rxp++;
    if (c == '\n')
    rxp = 0;
	
  #else
  	loop_until_bit_is_set(CHIP_UCSRA, CHIP_RXC);
	if (CHIP_UCSRA & _BV(CHIP_FE))
	  return _FDEV_EOF;
	if (CHIP_UCSRA & _BV(CHIP_DOR))
	  return _FDEV_ERR;
	c = CHIP_UDR;
	uart_putchar(c, stream);
#endif


  return c;
}

