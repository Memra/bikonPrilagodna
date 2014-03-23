#include "modbus_atmega.h"

#if defined _ATMEGA8_
#include "mega8.h"
#elif defined _ATMEGA16_
#include "mega16.h"
#elif defined _ATMEGA32_
#include "mega32.h"
#elif defined _ATMEGA64_
#include "mega64.h"
#elif defined _ATMEGA128_
#include "mega128.h"
#endif

#if defined _ATMEGA8_ || defined _ATMEGA16_ || defined _ATMEGA32_
#define __USART_RXC__ USART_RXC
#define __USART_TXC__ USART_TXC
#define __UDR__ UDR
#define __UBRRH__ UBRRH
#define __UBRRL__ UBRRL
#define __UCSRA__ UCSRA
#define __UCSRB__ UCSRB
#define __UCSRC__ UCSRC
#elif defined _ATMEGA64_ || defined _ATMEGA128_
#if defined _UART0_
#define __USART_RXC__ USART0_RXC
#define __USART_TXC__ USART0_TXC
#define __UDR__ UDR0
#define __UBRRH__ UBRR0H
#define __UBRRL__ UBRR0L
#define __UCSRA__ UCSR0A
#define __UCSRB__ UCSR0B
#define __UCSRC__ UCSR0C
#elif defined _UART1_              
#define __USART_RXC__ USART1_RXC
#define __USART_TXC__ USART1_TXC
#define __UDR__ UDR1
#define __UBRRH__ UBRR1H
#define __UBRRL__ UBRR1L
#define __UCSRA__ UCSR1A
#define __UCSRB__ UCSR1B
#define __UCSRC__ UCSR1C
#endif
#endif

#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

void (*DirPinFunction)(int val);
void (*ModbusMainFunction)(void);

// USART Receiver buffer
#define RX_BUFFER_SIZE 64
char rx_buffer[RX_BUFFER_SIZE];
unsigned char rx_wr_index,rx_rd_index,rx_counter;
// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;
// USART Receiver interrupt service routine
interrupt [__USART_RXC__] void usart_rx_isr(void)
{
    char status,data;
    status=__UCSRA__;
    data=__UDR__;
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
    {
        rx_buffer[rx_wr_index]=data;
        if (++rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
        if (++rx_counter == RX_BUFFER_SIZE)
        {
            rx_counter=0;
            rx_buffer_overflow=1;
        };
        
        if(ModbusMainFunction != 0)
        {
            ModbusMainFunction();
        }
    };
    
}

// USART Transmitter buffer
#define TX_BUFFER_SIZE 64
char tx_buffer[TX_BUFFER_SIZE];
unsigned char tx_wr_index,tx_rd_index,tx_counter;
// USART Transmitter interrupt service routine
interrupt [__USART_TXC__] void usart_tx_isr(void)
{
    if (tx_counter)
    {
        --tx_counter;
        __UDR__=tx_buffer[tx_rd_index];
        if (++tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
    }
    else
    {
        if(DirPinFunction != 0)
        {
            DirPinFunction(0);
        }
    }
}

void ModbusUartInit(void (*DirPinFunc)(int), void (*ModbusMainFunc)(void))
{
    DirPinFunction = DirPinFunc;
    ModbusMainFunction = ModbusMainFunc;
    
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART Receiver: On
    // USART Transmitter: On
    __UCSRA__ = 0x00;
    __UCSRB__ = 0xD8;
    __UCSRC__ = 0x86;
    __UBRRH__ = (MODBUS_OSC/(16*MODBUS_BAUDRATE)-1)>>8;
    __UBRRL__ = (MODBUS_OSC/(16*MODBUS_BAUDRATE)-1)&0xFF;
}

int ModbusUartGetChar(char *data)
{    
    if(rx_counter!=0)
    {
        *data=rx_buffer[rx_rd_index];
        if (++rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
    	#if (MODBUS_USE_INTERRUPT == 0)
	#asm("cli")
	#endif
	--rx_counter;
    	#if (MODBUS_USE_INTERRUPT == 0)
	#asm("sei")
	#endif

        return 1;
    }
    return 0;
}

int ModbusUartPutChar(char c)
{
    if(tx_counter != TX_BUFFER_SIZE)
    {
	
    	#if (MODBUS_USE_INTERRUPT == 0)
	#asm("cli")
	#endif
        if (tx_counter || ((__UCSRA__ & DATA_REGISTER_EMPTY)==0))
        {
            tx_buffer[tx_wr_index]=c;
            if (++tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
            ++tx_counter;
        }    
        else
        {
            if(DirPinFunction != 0)
            {
                DirPinFunction(1);
            }
            __UDR__=c;
        }
    	#if (MODBUS_USE_INTERRUPT == 0)
	#asm("sei")
	#endif

        return 1;
    }
    return 0;
}

int ModbusUartPutString(char *s, unsigned int count)
{
    int i;
    for(i = 0; i < count; i++)
    {
        if(ModbusUartPutChar(s[i]) == 0)
        {
            break;
        }
    }
    return i;
}
