/*
 * user_setting.h
 *
 *  Created on: 02-Jul-2019
 *      Author: poe
 */

#ifndef USER_SETTING_H_
#define USER_SETTING_H_


#define RD_PORT GPIOD
#define RD_PIN  GPIO_PIN_7

#define WR_PORT GPIOC
#define WR_PIN  GPIO_PIN_0

#define CD_PORT GPIOD          // RS PORT
#define CD_PIN  GPIO_PIN_5     // RS PIN

#define CS_PORT GPIOF
#define CS_PIN  GPIO_PIN_3

#define RESET_PORT GPIOF
#define RESET_PIN  GPIO_PIN_5

#define D0_PORT GPIOF
#define D0_PIN GPIO_PIN_12

#define D1_PORT GPIOD
#define D1_PIN GPIO_PIN_15

#define D2_PORT GPIOF
#define D2_PIN GPIO_PIN_15

#define D3_PORT GPIOE
#define D3_PIN GPIO_PIN_13

#define D4_PORT GPIOF
#define D4_PIN GPIO_PIN_14

#define D5_PORT GPIOE
#define D5_PIN GPIO_PIN_11

#define D6_PORT GPIOE
#define D6_PIN GPIO_PIN_9

#define D7_PORT GPIOF
#define D7_PIN GPIO_PIN_13



#define  WIDTH    ((uint16_t)320)
#define  HEIGHT   ((uint16_t)480)


/****************** delay in microseconds ***********************/
extern TIM_HandleTypeDef htim4;


void delay (uint32_t time);

// configure macros for the data pins.

/* First of all clear all the LCD_DATA pins i.e. LCD_D0 to LCD_D7
 * We do that by writing the HIGHER bits in BSRR Register
 *
 * For example :- To clear Pins B3, B4 , B8, B9, we have to write GPIOB->BSRR = 0b0000001100011000 <<16
 *
 *
 *
 * To write the data to the respective Pins, we have to write the lower bits of BSRR :-
 *
 * For example say the PIN LCD_D4 is connected to PB7, and LCD_D6 is connected to PB2
 *
 * GPIOB->BSRR = (data & (1<<4)) << 3.  Here first select 4th bit of data (LCD_D4), and than again shift left by 3 (Total 4+3 =7 i.e. PB7)
 *
 * GPIOB->BSRR = (data & (1<<6)) >> 4.  Here first select 6th bit of data (LCD_D6), and than again shift Right by 4 (Total 6-4 =2 i.e. PB2)
 *
 *
 */
  #define write_8(d) { \
   GPIOD->BSRR = 0b1000000000000000 << 16; \
   GPIOE->BSRR = 0b0010101000000000 << 16; \
   GPIOF->BSRR = 0b1111000000000000 << 16; \
   GPIOD->BSRR = (((d) & (1<<1)) << 14);\
   GPIOE->BSRR = (((d) & (1<<3)) << 10) \
               | (((d) & (1<<5)) << 6) \
			   | (((d) & (1<<6)) << 3); \
   GPIOF->BSRR = (((d) & (1<<0)) << 12) \
			   | (((d) & (1<<2)) << 13) \
			   | (((d) & (1<<4)) << 10) \
			   | (((d) & (1<<7)) << 6); \
    }


  /* To read the data from the Pins, we have to read the IDR Register
   *
   * Take the same example say LCD_D4 is connected to PB7, and LCD_D6 is connected to PB2
   *
   * To read data we have to do the following
   *
   * GPIOB->IDR & (1<<7) >> 3. First read the PIN (1<<7 means we are reading PB7) than shift it to the position, where it is connected to
   * and in this example, that would be 4 (LCD_D4). (i.e. 7-3=4)
   *
   * GPIOB->IDR & (1<<2) << 4. First read the PIN (1<<2 means we are reading PB2) than shift it to the position, where it is connected to
   * and in this case, that would be 6 (LCD_D6). (i.e. 2+4= 6). Shifting in the same direction
   *
   */
  #define read_8() (         (((GPIOF->IDR & (1<<12)) >> 12) \
                           | ((GPIOD->IDR & (1<<15)) >> 14) \
                           | ((GPIOF->IDR & (1<<15)) >> 13) \
                           | ((GPIOE->IDR & (1<<13)) >> 10) \
                           | ((GPIOF->IDR & (1<<14)) >> 10) \
                           | ((GPIOE->IDR & (1<<11)) >> 6) \
                           | ((GPIOE->IDR & (1<<9)) >> 3) \
                           | ((GPIOF->IDR & (1<<13)) >> 6)))



/********************* For 180 MHz *****************************/
//#define WRITE_DELAY { WR_ACTIVE8; }
//#define READ_DELAY  { RD_ACTIVE16;}


/************************** For 72 MHZ ****************************/
//#define WRITE_DELAY { }
//#define READ_DELAY  { RD_ACTIVE;  }


/************************** For 100 MHZ ****************************/
//#define WRITE_DELAY { WR_ACTIVE2; }
//#define READ_DELAY  { RD_ACTIVE4; }


/************************** For 216 MHZ ****************************/
#define WRITE_DELAY { WR_ACTIVE8; WR_ACTIVE8; } //216MHz
#define IDLE_DELAY  { WR_IDLE4;WR_IDLE4; }
#define READ_DELAY  { RD_ACTIVE16;RD_ACTIVE16;RD_ACTIVE16;}


/************************** For 48 MHZ ****************************/
//#define WRITE_DELAY { }
//#define READ_DELAY  { }


/*****************************  DEFINES FOR DIFFERENT TFTs   ****************************************************/

//#define SUPPORT_0139              //S6D0139 +280 bytes
//#define SUPPORT_0154              //S6D0154 +320 bytes
//#define SUPPORT_1289              //SSD1289,SSD1297 (ID=0x9797) +626 bytes, 0.03s
//#define SUPPORT_1580              //R61580 Untested
//#define SUPPORT_1963              //only works with 16BIT bus anyway
//#define SUPPORT_4532              //LGDP4532 +120 bytes.  thanks Leodino
//#define SUPPORT_4535              //LGDP4535 +180 bytes
//#define SUPPORT_68140             //RM68140 +52 bytes defaults to PIXFMT=0x55
//#define SUPPORT_7735
//#define SUPPORT_7781              //ST7781 +172 bytes
//#define SUPPORT_8230              //UC8230 +118 bytes
//#define SUPPORT_8347D             //HX8347-D, HX8347-G, HX8347-I, HX8367-A +520 bytes, 0.27s
//#define SUPPORT_8347A             //HX8347-A +500 bytes, 0.27s
//#define SUPPORT_8352A             //HX8352A +486 bytes, 0.27s
//#define SUPPORT_8352B             //HX8352B
//#define SUPPORT_8357D_GAMMA       //monster 34 byte
//#define SUPPORT_9163              //
//#define SUPPORT_9225              //ILI9225-B, ILI9225-G ID=0x9225, ID=0x9226, ID=0x6813 +380 bytes
//#define SUPPORT_9326_5420         //ILI9326, SPFD5420 +246 bytes
//#define SUPPORT_9342              //costs +114 bytes
//#define SUPPORT_9806              //UNTESTED
#define SUPPORT_9488_555            //costs +230 bytes, 0.03s / 0.19s
//#define SUPPORT_B509_7793         //R61509, ST7793 +244 bytes
//#define OFFSET_9327 32            //costs about 103 bytes, 0.08s




#endif /* USER_SETTING_H_ */
