/****************************************************************
 * Driver for ST7920 LCD Display 128x64                         *
 * Author: Petr Skryja      skryja.petr@seznam.cz               *
 * Date:   10.01.2018                                           *
 ***************************************************************/
#ifndef ST7920_H_INCLUDED
#define ST7920_H_INCLUDED

#include <stdbool.h>
#include "stm32f7xx_hal.h"

/****************************************************************************
*                                Display 128x64                             *
*                 AddrX --->                                                *
*                 0,1,2,3...      ...             ...              ...127   *
*                _\_|_/________________________________ _ _                 *
*     AddrY |  0 |................|................|.....        (row 0)    *
*           |  1 |................|................|...          (row 1)    *
*           V  2 |................|................|...          (row 2)    *
*                |................|................|...             .       *
*                .                                                  .       *
*                .                                                  .       *
*                .                                                          *
*                |................|................|...                     *
*             63 |________________|________________|___ _ _                 *
*                    0.subarray       1.subarray       2.subarray           *
*                    (0.column)       (1.column)       (2.column)           *
****************************************************************************/

#define ST7920_DISP_WIDTH       128u        /* width of display [pixels] */
#define ST7920_DISP_HEIGTH      64u         /* heigth of display [pixels] */
#define ST7920_PIX_IN_SUBARRAY  16u         /* number of pixels in subarray - 1 subarray = 16 pixels */

/* PIN mapping */


#define LCD_RS_Pin GPIO_PIN_8
#define LCD_RS_GPIO_Port GPIOC

#define LCD_RW_Pin GPIO_PIN_9
#define LCD_RW_GPIO_Port GPIOC

#define LCD_E_Pin GPIO_PIN_10
#define LCD_E_GPIO_Port GPIOC

#define LCD_DB4_Pin GPIO_PIN_11
#define LCD_DB4_GPIO_Port GPIOC
#define LCD_DB5_Pin GPIO_PIN_12
#define LCD_DB5_GPIO_Port GPIOC
#define LCD_DB6_Pin GPIO_PIN_2
#define LCD_DB6_GPIO_Port GPIOD
#define LCD_DB7_Pin GPIO_PIN_2
#define LCD_DB7_GPIO_Port GPIOG

#define LCD_PSB_Pin GPIO_PIN_3
#define LCD_PSB_GPIO_Port GPIOG

#define LCD_RST_Pin GPIO_PIN_7
#define LCD_RST_GPIO_Port GPIOD

typedef enum
{
    set_pixel,      /* logical operation OR */
    reset_pixel,    /* logical operation AND */
    xor_pixel,      /* logical operation XOR */
    rewrite_pixel,  /* rewrite pixel */
} pixel_action_en;

void ST7920_draw_8pixels(uint8_t AddrX, uint8_t AddrY, uint8_t Pixels, pixel_action_en PixelAction);
void ST7920l_draw_pixel(uint8_t AddrX, uint8_t AddrY, pixel_action_en PixelAction);
void ST7920_clear_graphic(void);
void ST7920_write_two_bytes(uint8_t Column, uint8_t Row, uint16_t TwoBytes);
void ST7920_set_addr_graphic(uint8_t Column, uint8_t Row);
void ST7920_init(void);
void ST7920_write_command(uint8_t Command);
void ST7920_write_data(uint8_t Data);
void ST7920_parallel_write_byte(bool RnW, bool nID, uint8_t BitRegister);
void delay_us(uint32_t Delay);

#endif /* ST7920_H_INCLUDED */
