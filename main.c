#include <reg51.h>
#include <uart.h>
#include <display.h>
#include <string.h>

// \r\n+CMT: "+919999999999","","15/02/20,23:13:00+20"L1ON

void gsmLink();
void clear();
void send_sms(unsigned int);
char keypad();


#define dat P1

unsigned char a[100];  
unsigned int i;
unsigned rx_flag;

sbit in1 = P3^4;
sbit in2 = P3^5;
sbit buzz = P3^6; 

sbit r4 = P2^7;
sbit r3 = P2^6;
sbit r2 = P2^5;
sbit r1 = P2^4;

sbit c1 = P2^3;
sbit c2 = P2^2;
sbit c3 = P2^1;

sbit close = P0^0;

unsigned char pwd[5];
unsigned int p;

void ser_int(void) interrupt 4
{
	if (RI == 1)
	{
		RI = 0;
		if (i < sizeof(a) - 1)  // Added overflow check
		{
			a[i] = SBUF;
			rx_flag = 1;
			i++;
		}
	}
	
	if (TI == 1)
	{
		TI = 0;
	}
}

void main() 
{ 
	  in1 = 0;
	  in2 = 0;
	  buzz = 0;
	
    lcd_init();
    TMOD = 0x20;
    TH1 = 0xFD;
    SCON = 0x50;
    IE = 0x90;
    TR1 = 1;

    lcd_cmd(0x80);
    lcd_str("VeeConn systems");
    delay(100);
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_str("Bank Locker");
	lcd_cmd(0xC0);
	lcd_str("   System");
	delay(200);
	lcd_cmd(0x01);
	lcd_cmd(0x80);
	lcd_str("GSM Link down!");
    gsmLink();

    while(1) {
		for(p = 0 ; p < 5 ; p++)
			pwd[p] = '\0';

		p = 0;

		while(p < 4)
		{
			pwd[p++] = keypad();
		}
		pwd[4] = '\0';

		if (0 == strcmp(pwd,"7878"))
		{
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_str("Enterred password");
			lcd_cmd(0xC0);
			lcd_str("is Right");
			send_sms(1);  // Correct password
			delay(100);

			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_str("Locker is opening");
			lcd_cmd(0xC0);
			lcd_str("                 ");
			in1 = 1;
			in2 = 0;

			delay(1000);
			in1 = 0;
			in2 = 0;
			 
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_str("Locker opened");
			lcd_cmd(0xC0);
			lcd_str("             ");
			
			while(close == 1);
			close = 1;
      
			delay(200);
			
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_str("Locker closing");
			lcd_cmd(0xc0);
			lcd_str("                ");
			in1 = 0;
			in2 = 1;
			delay(1000); 		
			in1 = 0;
			in2 = 0;
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_str("Locker closed!");
		}
		else
		{
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_str("Enterred password");
			lcd_cmd(0xc0);
			lcd_str(" is incorrect ");
			delay(1000);
			send_sms(0);  // Wrong password
			buzz = 1;
			delay(1000);
			buzz = 0;
		}
    }
}

void send_sms(unsigned int s)
{
	lcd_cmd(0x01);
	lcd_cmd(0x80);
	lcd_str("Sending SMS : ");
	delay(650);
	
	sendser_str("AT+CMGS=");
	sendser_char(0x22);
	sendser_str("+919999999999");
	sendser_char(0x22);
	sendser_char(0x0D);
	delay(650);

	if (s == 1)
	{
		sendser_str("Locker opened");
	}
	else
	{
		sendser_str("Invalid password attempt!");
	}
	  
	sendser_char(0x1A);
	delay(5);
	lcd_cmd(0x80);
	lcd_cmd(0x01);
	delay(20);

	lcd_cmd(0x80);
	lcd_str("SMS SENT");
	delay(20);
	lcd_data(0x01);

	delay(50);

	lcd_cmd(0x80);
	lcd_str("VEECONN SYSTEMS");
	delay(20);
	clear();

	delay(6500);
	lcd_cmd(0x01);
}

char keypad()
{
	char key = 0;
	while(1)
	{
		// Column 1
		c1 = 0; c2 = 1; c3 = 1;
		if(r1 == 0) { delay(20); while(r1==0); key = '1'; break; }
		if(r2 == 0) { delay(20); while(r2==0); key = '4'; break; }
		if(r3 == 0) { delay(20); while(r3==0); key = '7'; break; }
		if(r4 == 0) { delay(20); while(r4==0); key = '*'; break; }

		// Column 2
		c1 = 1; c2 = 0; c3 = 1;
		if(r1 == 0) { delay(20); while(r1==0); key = '2'; break; }
		if(r2 == 0) { delay(20); while(r2==0); key = '5'; break; }
		if(r3 == 0) { delay(20); while(r3==0); key = '8'; break; }
		if(r4 == 0) { delay(20); while(r4==0); key = '0'; break; }

		// Column 3
		c1 = 1; c2 = 1; c3 = 0;
		if(r1 == 0) { delay(20); while(r1==0); key = '3'; break; }
		if(r2 == 0) { delay(20); while(r2==0); key = '6'; break; }
		if(r3 == 0) { delay(20); while(r3==0); key = '9'; break; }
		if(r4 == 0) { delay(20); while(r4==0); key = '#'; break; }
	}
	return key;
}

void gsmLink()
{
	unsigned int retry = 0;

	// Try AT connection
	while(retry++ < 5)
	{
		i = 0;
		sendser_str("AT");
		sendser_char(0x0d);
		lcd_cmd(0x80);
		delay(2);

		if (a[0] == 'O' && a[1] == 'K')
		{
			lcd_str("GSM LINK Success!");
			break;
		}
		else
		{
			lcd_str("GSM LINK Not ok!");
		}
		clear();
		delay(650);
	}

	if (retry >= 5)
	{
		lcd_str("GSM Failed!");
		while(1);  // Stop further execution
	}

	clear();
	delay(650);

	// Set GSM to text mode
	retry = 0;
	while(retry++ < 5)
	{
		i = 0;
		lcd_cmd(0x01);
		sendser_str("AT+CMGF=1");
		sendser_char(0x0d);
		lcd_cmd(0x80);
		delay(2);

		if (a[0] == 'O' && a[1] == 'K')
		{
			lcd_str("GSM TEXT MODE");
			break;
		}
		else
		{
			lcd_str("CONFIG TEXT..");
		}
		clear();
		delay(650);
	}

	if (retry >= 5)
	{
		lcd_str("CMGF Failed!");
		while(1);
	}

	clear();
	delay(650);
}

void clear()
{
	unsigned int j = 0;
	for(j = 0 ; j < 100 ; j++)
	{
		a[j] = 0;
	}
	i = 0;
}
