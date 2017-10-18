/*************************************************************************
Title:    game_console Template
Initial Author:   David Jahshan
Extended by : (Jack Zheng) (521227)
Software: AVR-GCC 
Hardware: ATMEGA16 @ 8Mhz 

DESCRIPTION:
	Main and functions for Game Console

*************************************************************************/

#include "game_console.h" 
#include "util/delay.h" 

unsigned char lcd_buffer[XPIXEL][YPIXEL/8];
unsigned char test = 0x01;
unsigned char Input_Reg = 0;
unsigned char Bklt_Reg=0;


/*--------------------------------------
Function:	ISR(INT0_vect)
Input:		---
Output:		---
Description:Interrupt Service Routine(External Interrupt)
--------------------------------------*/
ISR(INT0_vect)
{
sei();
	if (~UP_BUTTON)
		{
			
			Input_Reg|=_BV(BTN_UP);
		}	
			
	if (~DOWN_BUTTON)
		{
			
			Input_Reg|=_BV(BTN_DOWN);
		}	

	if (~LEFT_BUTTON)
		{
			
			Input_Reg|=_BV(BTN_LEFT);
		}	


	if (~RIGHT_BUTTON)
		{
			
			Input_Reg|=_BV(BTN_RIGHT);
		}	

	if (~A_BUTTON)
		{
			
			Input_Reg|=_BV(BTN_A);
		}			
	if (~B_BUTTON)
		{
			
			Input_Reg|=_BV(BTN_B);
		}	
	if (~START_BUTTON)
		{
			
			Input_Reg|=_BV(BTN_START);
		}	
}

/*--------------------------------------
Function:	ISR(SPI_STC_vect)
Input:		---
Output:		---
Description:Initializes and clears lcd
--------------------------------------*/
ISR(SPI_STC_vect)
{
sei();
//SPI_COMPLETE(ON);
}


/*--------------------------------------
Function:	SPI_Init()
Input:		---
Output:		---
Description:Initializes and clears lcd
--------------------------------------*/

void SPI_Init(void)
{
	//Set Slave Slect Port to Output HIg
	SLAVE_SELECT_DIR(OUT);
	SLAVE_SELECT(ON);

	//Set MOSI and SCK output
	MISO_DIR(IN);
	MOSI_DIR(OUT);
	SCK_DIR(OUT);
	// Enable SPI in Master Mode with SCK = CK/4, CPHA = 1, CPOL=1, in Mode 3
    SPI_SETUP;
    //SPCR    = (1<<SPE)|(1<<MSTR)|(1<<CPHA)|(1<<CPOL)|(1<<SPR0)|(1<<SPR1);
}


/*--------------------------------------
Function:	SPI_Transmit()
Input:		cData
Output:		---
Description:Transmits data via SPI
--------------------------------------*/
void SPI_Transmit(unsigned char cData)
{
	SPDR = cData;	//Put data into SPI Data Register
	while(!(SPSR & (1<<SPIF)))	//Wait for transission to complete
		;
}

/*--------------------------------------
Function:	LCD_Command_TX()
Input:		cCommand
Output:		---
Description:Transmits a command byte to LCD
--------------------------------------*/
void LCD_Command_TX(unsigned char cCommand)
{
	cli();//Disable Global Interrupts
	LCD_CD(LOW);//Data Pulled LOW
	LCD_CS(LOW);//Pull Chip select low
	SPDR = cCommand;//Load data into SPI Data Register
	while(!(SPSR & (1<<SPIF)))	//Wait for transmission to complete
		;
	LCD_CS(ON); //Pull Chip select HIGH
	LCD_CD(HIGH); //Data Pulled HIGH
	sei(); //Enable Global Interrupts
}

/*--------------------------------------
Function:	LCD_Data_TX()
Input:		cData
Output:		---
Description:Transmits a data byte to LCD
--------------------------------------*/
void LCD_Data_TX(unsigned char cData)
{
	cli(); //Disable Global Interrupts
	LCD_CD(HIGH); //Data Pulled HIGH
	LCD_CS(LOW);//Pull Chip select low
	SPDR = cData;//Load data into SPI Data Register	
	while(!(SPSR & (1<<SPIF))) //Wait for transmission to complete
		;
	LCD_CS(HIGH);
	LCD_CD(LOW);
	sei();//Enable Global Interrupts
	return;
}



/*--------------------------------------
Function:	LCD_Select_Page()
Input:		page
Output:		---
Description:Sends page select command
--------------------------------------*/

void LCD_Select_Page(unsigned char page)
{
	unsigned char Page_Data;
	if (page > MAX_PAGES-1) //Checks if pages within bounds
	{
		page=MAX_PAGES-1;
	}

	Page_Data= 0xB0 | page;
	LCD_Command_TX(Page_Data);
	return;
}

/*--------------------------------------
Function:	LCD_Select_Column()
Input:		column
Output:		---
Description:Sends column select command
--------------------------------------*/

void LCD_Select_Column(unsigned char column)
{
	unsigned char Column_Data_MSB;
	unsigned char Column_Data_LSB;


	if (column > MAX_COLUMNS-1) //Checks if columns within bounds
	{
		column=MAX_COLUMNS-1;
	}

	column=column+LCD_OFFSET;	//Offset for 12/0clock Viewing

	Column_Data_LSB= CMD_COL_LSB |(column&0x0F);//Low nibble
	Column_Data_MSB= CMD_COL_MSB |(column >> 4); //High nibble
	LCD_Command_TX(Column_Data_LSB);//Sends LSB first
	LCD_Command_TX(Column_Data_MSB);
	return;
}


/*--------------------------------------
Function:	lcd_clear()
Input:		xs, ys, xe, ye
Output:		---
Description: Clears rectangle. 
Source: Application Notes for DOGS102 LED Display
Modified by Jack Zheng 19/10/16
--------------------------------------*/
void LCD_clear(unsigned char xs, unsigned char ys, unsigned char xe, unsigned char ye)
{
	//i,j are index variables
	//ps, pe are page start and endaddress
	//yr is restpixels
	unsigned char i=0,j=0, ps=0, pe=0, yr=0;

	ps=ys/8; //calculate startpage
	pe=ye/8; //calculate endpage
	//-------------Clear part of startpage--------------
	//Set column address
	LCD_Select_Column(xs);
	//set page adress
	LCD_Select_Page(ps);
	j=0xFF; //use j as buffer
	if (pe == ps) //if start and endadress are in same page
	{
		j=ye%8-0xFF; //calculate stop within first page
	}

	yr=ys%8; //calculate the start within first page
	for(i=xs; i<=xe; i++) //loop starting first colomn to last coloumn
	{

		lcd_buffer[i][ps]&=j>>(8-yr);     //clear the buffer
		LCD_Data_TX(lcd_buffer[i][ps]); //send the changed pages of the buffer to the display
	}

	//-------------Clear part of endpage----------------
	//Set column adress
	LCD_Select_Column(xs);
	//set page adress
	LCD_Select_Page(pe);
	yr=ye%8; //calculate the stop within last page

	for(i=xs; i<=xe; i++) //loop starting first colomn to last coloumn
	{
		lcd_buffer[i][pe]&=(0xFF<<(yr+1)); //clear the buffer
		LCD_Data_TX(lcd_buffer[i][pe]);  //send the changed pages of the buffer to the display
	}


	//-------------------Clear middle pages----------------------
	for(j=ps+1; j<pe; j++) //loop starting first middle page to last middle page
	{
		for(i=xs; i<=xe; i++) //loop starting first colomn to last coloumn
		{
			//set page adress
			LCD_Select_Page(j);
			//Set coloumn adress
			LCD_Select_Column(i);
			lcd_buffer[i][j]=0x00; //clear the buffer
			LCD_Data_TX(0x00);   //clear display same as lcd_send_data(lcd_buffer[i][j]);
		}
	}

}


/*--------------------------------------
Function:	LCD_Pixel()
Input:		x, y, state
Output:		---
Description: Sets or clears a single pixel
--------------------------------------*/


void LCD_Pixel(uint8_t x, uint8_t y, uint8_t state)
{
	uint8_t i=0,j=0, page=0, yr=0;
	page=y/8;
	yr=y%8;
	
	if (state == ON)
	{
	lcd_buffer[x][page]|=_BV(yr);
	}
	if (state == OFF)
	{
		lcd_buffer[x][page]&=0xFF-_BV(yr);
	}

	for(j=0; j<MAX_PAGES; j++) //loop starting first middle page to last middle page
	{
	
		for(i=0; i<MAX_COLUMNS; i++)	//loop starting first colomn to last coloumn
		{
			LCD_Select_Column(i);//Set coloumn adress
			LCD_Select_Page(j);//set page adress
			LCD_Data_TX(lcd_buffer[i][j]);   //fill display same as lcd_send_data(lcd_buffer[i][j]);
		}
	}
	
}

/*--------------------------------------
Function:	LCD_Init()
Input:		---
Output:		---
Description: Initializes and clears lcd
--------------------------------------*/

void LCD_Init(void)
{
	//---------Setup Ports--------------//
	LCD_BKLT(OFF);//Turn off LCD output
	LCD_BKLT_DIR(OUT);//Set up LCD Backlight as Output
	LCD_CS(ON);//Turn ON LCD_CS
	LCD_CS_DIR(OUT);//Set up LCD Backlight as Output
	LCD_CD(OFF);//Turn off CD
	LCD_CD_DIR(OUT);//Set up LCD Backlight as Output
	LCD_RST_DIR(OUT);//Set up LCD RESET

	//Cycle Reset
	LCD_RST(ON);//Turn ON Reset
	_delay_ms(5);
	LCD_RST(OFF);//Turn ON Reset
	_delay_ms(5);
	LCD_RST(ON);//Turn ON Reset
	_delay_ms(10);

	//Initialize LCD
	LCD_Command_TX(0x40); //set scroll line 
	LCD_Command_TX(0xA0); //12 oclock viewing angle
	LCD_Command_TX(0xC8);
	LCD_Command_TX(0xA4); //All pixels off
	LCD_Command_TX(0xA6);
	_delay_ms(120);
	LCD_Command_TX(0xA2);
	LCD_Command_TX(0x2F);
	LCD_Command_TX(0x27);
	LCD_Command_TX(0x81);
	LCD_Command_TX(0x10);
	LCD_Command_TX(0xFA);
	LCD_Command_TX(0x90);
	LCD_Command_TX(0xAF);
	LCD_clear(0, 0, XPIXEL-1, YPIXEL-1);
}

/*--------------------------------------
Function:	Backlight_Init()
Input:		---
Output:		---
Description:Setup PWM for Backlight
--------------------------------------*/
void Backlight_Init()
{
	//Setup Timer 2 in non inverting mode at Fosc/8 

	TCCR2|=(1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS01);//S
	DUTY_CYCLE(0x00);
	LCD_BKLT(ON);
}

/*--------------------------------------
Function:	ADC_Read()
Input:		---
Output:		Battery Voltage
Description: Reads Battery Voltage
--------------------------------------*/
uint16_t ADC_Read(unsigned char channel)
{
	channel &= 0b00000111;
	ADMUX = (ADMUX & 0xF8)|channel; // clears the bottom 3 bits before ORing
	ADCSRA |= (1<<ADSC);//Start conversion
	while(ADCSRA & (1<<ADSC));
	return(ADC);
}

/*--------------------------------------
Function:	ADC_Init()
Input:		---
Output:		---
Description:Setup PWM for Backlight
--------------------------------------*/
void ADC_Init(){
     ADMUX=(1<<REFS1)|(1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0);
     ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
     //Enables conversion
     ADCSRA|=(1<<ADSC);
	 //Discard first reading
	uint16_t discard;
	discard=ADC_Read(0x07);

}

/*--------------------------------------
Function:	FM_Init()
Input:		---
Output:		---
Description:Setup FRAM
--------------------------------------*/
void FM_Init(void)
{
    FM_CS(HIGH);
    FM_CS_DIR(OUT);
    FM_WP(HIGH);//
    FM_WP_DIR(OUT);
    FM_HOLD(HIGH);//Always high since we're not using
    FM_HOLD_DIR(OUT);
	_delay_ms(1);
}

/*--------------------------------------
Function:	FM_Write()
Input:		Address_MSB, Address_LSB, cData
Output:		---
Description: Writes cData to FRAM at address specified
--------------------------------------*/
void FM_Write(unsigned char Address_MSB, unsigned char Address_LSB, unsigned char cData)
{
	cli();
	//FM_WP(HIGH);
	FM_CS(LOW); //Pull Chip select low
	SPI_Transmit(FM_ENABLEWRITE); //0x06
 	FM_CS(HIGH);
	_delay_ms(1);
	FM_CS(LOW);
	SPI_Transmit(FM_WRITEDATA);
	SPI_Transmit(Address_MSB);//Most significant 3 bytes of address.
	SPI_Transmit(Address_LSB);//LS 8 bytes of address.
	SPI_Transmit(cData);//Data
	FM_CS(HIGH);
	//FM_WP(LOW);
	sei();
}

/*--------------------------------------
Function:	FM_Read()
Input:		Address_MSB, Address_LSB
Output:		---
Description: Reads data from FRAM at address specified
--------------------------------------*/
unsigned char FM_Read(unsigned char Address_MSB, unsigned char Address_LSB)
{
	FM_CS(LOW);
	SPI_Transmit(FM_READDATA);
	SPI_Transmit(Address_MSB);//Most significant 3 bytes of address.
	SPI_Transmit(Address_LSB);//LS 8 bytes of address.
	SPI_Transmit(ON);
	unsigned char cData=SPDR;
	FM_CS(HIGH);
	return cData;
}

/*--------------------------------------
Function:	Inputs_Init()
Input:		---
Output:		---
Description: Sets up input ports
--------------------------------------*/
void Inputs_Init(void)
{
 	//---------Inputs--------------//
	UP_BUTTON_DIR(IN); 
	UP_BUTTON_PULLUP(ON);
	DOWN_BUTTON_DIR(IN); 
	DOWN_BUTTON_PULLUP(ON);
	LEFT_BUTTON_DIR(IN);
	LEFT_BUTTON_PULLUP(ON);
	RIGHT_BUTTON_DIR(IN); 
	RIGHT_BUTTON_PULLUP(ON);
	A_BUTTON_DIR(IN); 
	A_BUTTON_PULLUP(ON);
	B_BUTTON_DIR(IN); 
	B_BUTTON_PULLUP(ON);
	START_BUTTON_DIR(IN); 
	START_BUTTON_PULLUP(ON);
}

/*--------------------------------------
Function:	Int_Init()
Input:		---
Output:		---
Description: Sets up interrupts
--------------------------------------*/
void Int_Init(void)
{
	INT0_SET(ON);
	ISC01_SET(ON);
	ISC00_SET(ON);
}
/*--------------------------------------
Function:	LED_Init()
Input:		---
Output:		---
Description: Sets up LED
--------------------------------------*/
void LED_Init(void)
{
	BAT_LOW_LED(OFF); //Make sure it is off before changing direction
	BAT_LOW_LED_DIR(OUT); //Set BATTERY LED I/Os as outputs.
	BAT_LOW_LED(ON);
}







int main(void)
{
	//--------Initializations---------//
	LED_Init();
	Int_Init();//Turns on external interrupts, for rising edge.
	Inputs_Init();
	SPI_Init();//initialize SPI
	LCD_Init();
	Backlight_Init(); //Init PWM 
	ADC_Init();
	FM_Init();
	
	//---------Setup Variables--------------//
	unsigned char draw=0;
	unsigned char pixel_y=FM_Read(PIXEL_Y_ADDR_MSB,PIXEL_Y_ADDR_LSB);
	unsigned char pixel_x=FM_Read(PIXEL_X_ADDR_MSB, PIXEL_X_ADDR_LSB);
			

	while (TRUE)//Master loop always true so always loop
	{
		sei(); //enable global interrupts
		if (!draw)
		{
			LCD_Pixel(pixel_x, pixel_y, OFF);
		}		

		if (Input_Reg & _BV(BTN_UP))
		{
			if ( pixel_y>0)
			{
				pixel_y--;
				//_delay_ms(50);
			}
			Input_Reg&=(~_BV(BTN_UP));//Clear Input Flag Register
		}
		if (Input_Reg & _BV(BTN_DOWN))
		{
			if ( pixel_y<YPIXEL)
			{
				pixel_y++;
				//_delay_ms(50);
			}
			Input_Reg&=(~_BV(BTN_DOWN));//Clear Input Flag Register
		}		
		if (Input_Reg & _BV(BTN_LEFT))
		{
			if (pixel_x>0)
			{
			
				pixel_x--;
				//_delay_ms(50);
			}
			Input_Reg&=(~_BV(BTN_LEFT));//Clear Input Flag Register
		}
		if (Input_Reg & _BV(BTN_RIGHT))
		{
			if (pixel_x<XPIXEL)
			{
			
				pixel_x++;
				//_delay_ms(50);
			}
			Input_Reg&=(~_BV(BTN_RIGHT));//Clear Input Flag Register
		}
		if (Input_Reg & _BV(BTN_A))
		{
			draw=(~draw);// Toggles Draw Flag
			Input_Reg&=(~_BV(BTN_A));//Clear Input Flag Register
			
		}

		if (Input_Reg & _BV(BTN_B))
		{
			LCD_clear(0, 0, XPIXEL-1, YPIXEL-1);
			pixel_x=MID_XPIXEL;
			pixel_y=MID_YPIXEL;
			BAT_LOW_LED(ON);
			_delay_ms(50);
			BAT_LOW_LED(OFF);
			Input_Reg&=(~_BV(BTN_B));//Clear Input Flag Register
		}
		if (Input_Reg & _BV(BTN_START))
		{
			if (Bklt_Reg<BKLT_MAX)
			{
				Bklt_Reg=Bklt_Reg+1;
			}
			else
			{
				Bklt_Reg=0;
			}
			Input_Reg&=(~_BV(BTN_START));
		}
		if (Bklt_Reg)
		{
			
			DUTY_CYCLE(Bklt_Reg*BKLT_LEVEL); // Set PWM to 1/5 setpoints;
		}
		else
		{
			DUTY_CYCLE(0); // Set PWM to Zero;
			LCD_BKLT(OFF);
		}
		
		//Write Current Cursor Position
		LCD_Pixel(pixel_x, pixel_y, ON);

		//Save Cursor Position
		FM_Write(PIXEL_X_ADDR_MSB, PIXEL_X_ADDR_LSB, pixel_x);
		FM_Write(PIXEL_Y_ADDR_MSB, PIXEL_Y_ADDR_LSB, pixel_y);

		if (ADC_Read(BATT_PORT)<BATT_LOW_LEVEL)
		{
			BAT_LOW_LED(ON);
		}
		else
		{
			BAT_LOW_LED(OFF);
		}

	}

}
