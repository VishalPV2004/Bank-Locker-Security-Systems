#include<reg51.h>
#include<uart.h>
#include<display.h>


void sendser_char(unsigned char c)
{
	SBUF = c;
	while(TI == 0);
	TI = 0;
}

void sendser_str(unsigned char *c)
{
	while(*c)
	{
		sendser_char(*c++);
		delay(2);
	}
}
