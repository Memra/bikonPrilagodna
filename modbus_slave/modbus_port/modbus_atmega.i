
void ModbusUartInit(void (*DirPinFunc)(int), void (*ModbusMainFunc)(void));
int ModbusUartGetChar(char *data);
int ModbusUartPutChar(char c);
int ModbusUartPutString(char *s, unsigned int count);

#pragma used+
sfrb TWBR=0;
sfrb TWSR=1;
sfrb TWAR=2;
sfrb TWDR=3;
sfrb ADCL=4;
sfrb ADCH=5;
sfrw ADCW=4;      
sfrb ADCSRA=6;
sfrb ADMUX=7;
sfrb ACSR=8;
sfrb UBRRL=9;
sfrb UCSRB=0xa;
sfrb UCSRA=0xb;
sfrb UDR=0xc;
sfrb SPCR=0xd;
sfrb SPSR=0xe;
sfrb SPDR=0xf;
sfrb PIND=0x10;
sfrb DDRD=0x11;
sfrb PORTD=0x12;
sfrb PINC=0x13;
sfrb DDRC=0x14;
sfrb PORTC=0x15;
sfrb PINB=0x16;
sfrb DDRB=0x17;
sfrb PORTB=0x18;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   
sfrb UBRRH=0x20;
sfrb UCSRC=0X20;
sfrb WDTCR=0x21;
sfrb ASSR=0x22;
sfrb OCR2=0x23;
sfrb TCNT2=0x24;
sfrb TCCR2=0x25;
sfrb ICR1L=0x26;
sfrb ICR1H=0x27;
sfrw ICR1=0x26;   
sfrb OCR1BL=0x28;
sfrb OCR1BH=0x29;
sfrw OCR1B=0x28;  
sfrb OCR1AL=0x2a;
sfrb OCR1AH=0x2b;
sfrw OCR1A=0x2a;  
sfrb TCNT1L=0x2c;
sfrb TCNT1H=0x2d;
sfrw TCNT1=0x2c;  
sfrb TCCR1B=0x2e;
sfrb TCCR1A=0x2f;
sfrb SFIOR=0x30;
sfrb OSCCAL=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb TWCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-

#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#endasm

void (*DirPinFunction)(int val);
void (*ModbusMainFunction)(void);

char rx_buffer[64];
unsigned char rx_wr_index,rx_rd_index,rx_counter;

bit rx_buffer_overflow;

interrupt [12] void usart_rx_isr(void)
{
char status,data;
status=UCSRA;
data=UDR;
if ((status & ((1<<4) | (1<<2) | (1<<3)))==0)
{
rx_buffer[rx_wr_index]=data;
if (++rx_wr_index == 64) rx_wr_index=0;
if (++rx_counter == 64)
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

char tx_buffer[64];
unsigned char tx_wr_index,tx_rd_index,tx_counter;

interrupt [14] void usart_tx_isr(void)
{
if (tx_counter)
{
--tx_counter;
UDR=tx_buffer[tx_rd_index];
if (++tx_rd_index == 64) tx_rd_index=0;
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

UCSRA = 0x00;
UCSRB = 0xD8;
UCSRC = 0x86;
UBRRH = (7372800/(16*115200)-1)>>8;
UBRRL = (7372800/(16*115200)-1)&0xFF;
}

int ModbusUartGetChar(char *data)
{    
if(rx_counter!=0)
{
*data=rx_buffer[rx_rd_index];
if (++rx_rd_index == 64) rx_rd_index=0;
--rx_counter;

return 1;
}
return 0;
}

int ModbusUartPutChar(char c)
{
if(tx_counter != 64)
{

if (tx_counter || ((UCSRA & (1<<5))==0))
{
tx_buffer[tx_wr_index]=c;
if (++tx_wr_index == 64) tx_wr_index=0;
++tx_counter;
}    
else
{
if(DirPinFunction != 0)
{
DirPinFunction(1);
}
UDR=c;
}

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
