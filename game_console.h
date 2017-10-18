/*************************************************************************
Title:    Game Console Template Header File
Inital Author:   David Jahshan
Extended by : (PUT YOUR NAME HERE) (PUT YOUR STUDENT NUMBER HERE) 
Software: AVR-GCC 
Hardware: ATMEGA16 @ 8Mhz 

DESCRIPTION:
	Macros for Game Console

*************************************************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define byte unsigned char 

/*ON OFF*/
#define ON 0xFF
#define OFF 0x00
#define HIGH 0xFF
#define LOW 0x00
#define IN 0x00
#define OUT 0xFF
#define ALL 0xFF
#define TRUE 1
#define FALSE 0
#define FORWARD 0x00
#define BACK 0xFF

/*SET and GET MACRO*/
#define SET(PORT,MASK,VALUE) PORT = ((MASK & VALUE) | (PORT & ~MASK))
#define GET(PORT,MASK) PORT & MASK

//Devices Direction MACROs

//LED
#define BAT_LOW_LED_DIR(DIR) SET(DDRB,_BV(PB0),DIR)

//BUTTONS
#define UP_BUTTON_DIR(DIR) SET(DDRA,_BV(PA4),DIR)
#define DOWN_BUTTON_DIR(DIR) SET(DDRC,_BV(PC7),DIR)
#define LEFT_BUTTON_DIR(DIR) SET(DDRA,_BV(PA6),DIR)
#define RIGHT_BUTTON_DIR(DIR) SET(DDRA,_BV(PA5),DIR)
#define A_BUTTON_DIR(DIR) SET(DDRD,_BV(PD5),DIR)
#define B_BUTTON_DIR(DIR) SET(DDRD,_BV(PD4),DIR)
#define START_BUTTON_DIR(DIR) SET(DDRD,_BV(PD0),DIR)
//LCD
#define LCD_CD_DIR(DIR) SET(DDRD,_BV(PD6),DIR)
#define LCD_BKLT_DIR(DIR) SET(DDRD,_BV(PD7),DIR)	
#define LCD_CS_DIR(DIR) SET(DDRC,_BV(PC0),DIR)
#define LCD_RST_DIR(DIR) SET(DDRC,_BV(PC1),DIR)
//SPI
#define SLAVE_SELECT_DIR(DIR) SET(DDRB,_BV(PB4),DIR)	
#define MOSI_DIR(STATE) SET(DDRB,_BV(PB5),STATE)
#define MISO_DIR(STATE) SET(DDRB,_BV(PB6),STATE)
#define SCK_DIR(STATE) SET(DDRB,_BV(PB7),STATE)
//FRAM
#define FM_CS_DIR(DIR) SET(DDRB,_BV(PB3),DIR)	
#define FM_WP_DIR(DIR) SET(DDRB,_BV(PB2),DIR)
#define FM_HOLD_DIR(DIR) SET(DDRB,_BV(PB1),DIR)

//Devices Outputs
//LED
#define BAT_LOW_LED(STATE) SET(PORTB,_BV(PB0),~STATE)//NOTE this is opposite polarity.
//LCD
#define LCD_CD(STATE) SET(PORTD,_BV(PD6),STATE)
#define LCD_BKLT(STATE) SET(PORTD,_BV(PD7),STATE)
#define LCD_CS(STATE) SET(PORTC,_BV(PC0),STATE)
#define LCD_RST(STATE) SET(PORTC,_BV(PC1),STATE)
//SPI
#define SLAVE_SELECT(STATE) SET(PORTB,_BV(PB4),STATE)
//FRAM
#define FM_CS(STATE) SET(PORTB,_BV(PB3),STATE)
#define FM_WP(STATE) SET(PORTB,_BV(PB2),STATE)
#define FM_HOLD(STATE) SET(PORTB,_BV(PB1),STATE)

//Device Pullups
#define UP_BUTTON_PULLUP(STATE) SET(PORTA,_BV(PA4),STATE)
#define DOWN_BUTTON_PULLUP(STATE) SET(PORTC,_BV(PC7),STATE)
#define LEFT_BUTTON_PULLUP(STATE) SET(PORTA,_BV(PA6),STATE)
#define RIGHT_BUTTON_PULLUP(STATE) SET(PORTA,_BV(PA5),STATE)
#define A_BUTTON_PULLUP(STATE) SET(PORTD,_BV(PD5),STATE)
#define B_BUTTON_PULLUP(STATE) SET(PORTD,_BV(PD4),STATE)
#define START_BUTTON_PULLUP(STATE) SET(PORTD,_BV(PD0),STATE)

//Devices Inputs
#define UP_BUTTON GET(PINA,_BV(PA4))
#define DOWN_BUTTON GET(PINC,_BV(PC7))
#define LEFT_BUTTON GET(PINA,_BV(PA6))
#define RIGHT_BUTTON GET(PINA,_BV(PA5))
#define A_BUTTON GET(PIND,_BV(PD5))
#define B_BUTTON GET(PIND,_BV(PD4))
#define START_BUTTON GET(PIND,_BV(PD0))

//Interrupts
//#define INT0(STATE) SET(GICR,_BV(INT0), STATE)
//#define ISC01(STATE) SET(MCUCR,_BV(ISC01), STATE)
//#define ISC00(STATE) SET(MCUCR,_BV(ISC00), STATE)

//SPI
#define SPI_COMPLETE(STATE) SET(SPSR,_BV(SPIF), STATE)
#define SPI_INTERRUPTS(STATE) SET(SPCR,_BV(SPIE), STATE)
#define SPI_SETUP SPCR    = (1<<SPE)|(1<<MSTR)|(1<<CPHA)|(1<<CPOL)|(1<<SPR0)|(1<<SPR1)


//Setup LCD SPI Ports as Outputs
#define DDR_SPI DDRB
#define DD_MOSI PB5 
#define DD_SCK PB7

//Try moving to header then commenting out INT0 definitions.
//Perhaps calling .h doesn't carry through, only local scope
//#define INT0 6
//#define ISC00 0
//#define ISC01 1
#define INT0_SET(STATE) SET(GICR,_BV(INT0), STATE)
#define ISC01_SET(STATE) SET(MCUCR,_BV(ISC01), STATE)
#define ISC00_SET(STATE) SET(MCUCR,_BV(ISC00), STATE)

#define MAX_PAGES 8
#define MAX_COLUMNS 102
#define CMD_PAGE 0xB0 
#define CMD_COL_LSB 0x00 
#define CMD_COL_MSB 0x10 
#define MSB_MASK 0x0F
#define XPIXEL 102
#define YPIXEL 64
#define MID_XPIXEL 51
#define MID_YPIXEL 32
#define LCD_OFFSET 30

//Input Flag Register
#define BTN_UP 0
#define BTN_DOWN 1
#define BTN_LEFT 2
#define BTN_RIGHT 3
#define BTN_A 4
#define BTN_B 5
#define BTN_START 6
#define LED_BATT 7

//PWM

#define BKLT_MAX 6
#define DUTY_CYCLE(STATE) OCR2=STATE
#define BKLT_LEVEL 51

//ADC
#define BATT_PORT 0x07
#define BATT_LOW_LEVEL 441

//FRAM
#define FM_ENABLEWRITE 0x06//0b0000 0110
#define FM_DISABLEWRITE 0x04//0b0000 0100
#define FM_READDATA 0x03//0b0000 0011
#define FM_WRITEDATA 0x02//0b0000 0010
//FRAM Addresses
#define PIXEL_Y_ADDR_MSB 0x00//0b0000 0000
#define PIXEL_Y_ADDR_LSB 0x0F//0b0000 0001
#define PIXEL_X_ADDR_MSB 0x00//0b0000 0000
#define PIXEL_X_ADDR_LSB 0xFF//0b0000 0010
