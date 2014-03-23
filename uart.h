#ifndef UART_H
#define	UART_H

#ifndef FCY
#define FCY 11059200ULL
#endif

#define UART_MODULE_ONE 1
#define UART_MODULE_TWO 2

void initUart1(long);
void initUart2(long);

void sendUart1(unsigned char);
void sendUart2(unsigned char);

unsigned char getUart1(void);
unsigned char getUart2(void);
unsigned char UART1_checkRx(void);
void flush(void);

extern void reportCoord(void);

#endif	/* UART_H */

