/**
 * Copyright (c) 2017, Łukasz Marcin Podkalicki <lpodkalicki@gmail.com>
 * The ASM code is based on Ralph Doncaster's project (https://github.com/nerdralph/nerdralph/tree/master/avr/libs/bbuart)
 * Software UART for ATtiny13
 */

#if defined(ARDUINO)&defined(ARDUINO_AVR_ATTINY13)
  #ifndef	_UART_H_
  #define	_UART_H_

  //#define	UART_RX_ENABLED		(1) // Enable UART RX
  //#define	UART_TX_ENABLED		(1) // Enable UART TX

  #ifndef F_CPU
  # define        F_CPU           (1200000UL) // 1.2 MHz
  #endif  /* !F_CPU */

  #if defined(UART_TX_ENABLED) && !defined(UART_TX)
  # define        UART_TX         PB3 // Use PB3 as TX pin
  #endif  /* !UART_TX */

  #if defined(UART_RX_ENABLED) && !defined(UART_RX)
  # define        UART_RX         PB4 // Use PB4 as RX pin
  #endif  /* !UART_RX */

  #if (defined(UART_TX_ENABLED) || defined(UART_RX_ENABLED)) && !defined(UART_BAUDRATE)
  # define        UART_BAUDRATE   (19200)
  #endif  /* !UART_BAUDRATE */

  #define	TXDELAY         	(int)(((F_CPU/UART_BAUDRATE)-7 +1.5)/3)
  #define RXDELAY         	(int)(((F_CPU/UART_BAUDRATE)-5 +1.5)/3)
  #define RXDELAY2        	(int)((RXDELAY*1.5)-2.5)
  #define RXROUNDED       	(((F_CPU/UART_BAUDRATE)-5 +2)/3)
  #if RXROUNDED > 127
  # error Low baud rates are not supported - use higher, UART_BAUDRATE
  #endif

  char uart_getc(void);
  void uart_putc(char c);
  void uart_putu(uint16_t x);
  void uart_puts(const char *s);

  #endif	/* !_UART_H_ */
#endif
