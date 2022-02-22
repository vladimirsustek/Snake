/****************************************************************
 * Driver for ST7920 LCD Display 128x64                         *
 * Author: Petr Skryja      skryja.petr@seznam.cz               *
 * Date:   10.01.2018                                           *
 ***************************************************************/
#include "ST7920.h"

//#include "stm32f7xx_it.h"


#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA/* Display status */

#define ST7920_DISPLAY_CONTROL  0x08
#define ST7920_DISPLAY_ON       0x04
#define ST7920_CURSOR_ON        0x02    /*only for text mode of ST7920*/
#define ST7920_BLINK_ON         0x01    /*only for text mode of ST7920*/

/* Display clear */
#define ST7920_DISPLAY_CLEAR    0x01

/* Function set*/
#define ST7920_FUNCTION_SET     0x20
#define ST7920_8BIT_BUS         0x10
#define ST7920_EX_INSTRUCTION   0x04
#define ST7920_GRAPHIC_ON       0x02

/* Entry mode */
#define ST7920_ENTRY_MODE       0x04
#define ST7920_AC_INCREASE      0x02
#define ST7920_DISPLAY_SHIFT    0x01

/* GDRAM */
#define ST7920_SET_GDRAM_ADDR   0x80

uint16_t ST7920_LCD_bit_array[ST7920_DISP_WIDTH / 16u][ST7920_DISP_HEIGTH];  /* Image of display */

/* Draw one byte (8pixels) on display in any position with the desired action (set/reset/xor/rewrite) */
/* This function allows for a faster rendering of eight pixels */
/* AddrX <0-119> AddrY <0-63> */
void ST7920_draw_8pixels(uint8_t AddrX, uint8_t AddrY, uint8_t Pixels, pixel_action_en PixelAction)
{
    const uint8_t Row = AddrY;
    uint8_t Column = AddrX / ST7920_PIX_IN_SUBARRAY;
    if((AddrX <= ST7920_DISP_WIDTH - 16u - 1u) && (AddrY < ST7920_DISP_HEIGTH))
    {
        uint16_t SubArray1 = ST7920_LCD_bit_array[Column][Row];
        uint16_t SubArray2 = ST7920_LCD_bit_array[Column + 1u][Row];

        uint8_t PixelsPosition = AddrX % ST7920_PIX_IN_SUBARRAY; /* get subarray number*/

        const uint32_t PixelsHandler = (uint32_t)Pixels << (24u - PixelsPosition); /* shift byte/pixels to the left */
        const uint32_t ClearPixelsHandler = 0xFF << (24u - PixelsPosition); /*shift byte/pixels for rewrite to the left*/

        switch(PixelAction)
        {
            case set_pixel:
                            SubArray1 = SubArray1 | ((PixelsHandler >> 16u) & 0xFFFF); /* shift and set pixels to the position in subarrays */
                            SubArray2 = SubArray2 | (PixelsHandler & 0xFFFF);          /* shift and set pixels to the position in subarrays */
                            break;

            case reset_pixel:
                            SubArray1 = SubArray1 & ~((PixelsHandler >> 16u) & 0xFFFF);
                            SubArray2 = SubArray2 & ~(PixelsHandler & 0xFFFF);
                            break;

            case xor_pixel:
                            SubArray1 = SubArray1 ^ ((PixelsHandler >> 16u) & 0xFFFF);
                            SubArray2 = SubArray2 ^ (PixelsHandler & 0xFFFF);
                            break;

            case rewrite_pixel:
                            SubArray1 = SubArray1 & ~((ClearPixelsHandler >> 16u) & 0xFFFF); /* clear pixels befor set (OR) pixel operation */
                            SubArray2 = SubArray2 & ~(ClearPixelsHandler & 0xFFFF);          /* clear pixels befor set (OR) pixel operation */
                            SubArray1 = SubArray1 | ((PixelsHandler >> 16u) & 0xFFFF);
                            SubArray2 = SubArray2 | (PixelsHandler & 0xFFFF);
                            break;

            default:
                    break;
        }

        ST7920_write_two_bytes(Column, Row, SubArray1);
        ST7920_write_two_bytes(Column + 1u, Row, SubArray2);
    }
    /* last subarray (16pixels) */
    else if((AddrX > ST7920_DISP_WIDTH - 16u - 1u) && (AddrX <= ST7920_DISP_WIDTH - 8u) && (AddrY < ST7920_DISP_HEIGTH))
    {
        uint16_t SubArray1 = ST7920_LCD_bit_array[Column][Row];

        uint8_t PixelsPosition = AddrX % ST7920_PIX_IN_SUBARRAY;

        const uint16_t PixelsHandler = (uint16_t)Pixels << (8u - PixelsPosition);
        const uint16_t ClearPixelsHandler = (uint16_t)0xFF << (8u - PixelsPosition);

        switch(PixelAction)
        {
            case set_pixel:
                            SubArray1 = SubArray1 | (PixelsHandler);
                            break;

            case reset_pixel:
                            SubArray1 = SubArray1 & ~(PixelsHandler);
                            break;

            case xor_pixel:
                            SubArray1 = SubArray1 ^ PixelsHandler;
                            break;

            case rewrite_pixel:
                            SubArray1 = SubArray1 & ~(ClearPixelsHandler);
                            SubArray1 = SubArray1 | (PixelsHandler);
                            break;

            default:
                    break;
        }

        ST7920_write_two_bytes(Column, Row, SubArray1);
    }
}

/* write one pixel with the desired action (set/reset/xor/rewrite) */
/* AddrX <0-127> AddrY <0-63> */
void ST7920l_draw_pixel(uint8_t AddrX, uint8_t AddrY, pixel_action_en PixelAction)
{
    if((AddrX < ST7920_DISP_WIDTH) && (AddrY < ST7920_DISP_HEIGTH))
    {
        const uint8_t Row = AddrY;
        const uint8_t Column = AddrX / ST7920_PIX_IN_SUBARRAY;

        uint16_t SubArray = ST7920_LCD_bit_array[Column][Row];

        const uint16_t PixelPosition = AddrX % ST7920_PIX_IN_SUBARRAY;
        const uint16_t PixelHandler = 0x8000 >> PixelPosition;

        switch(PixelAction)
        {
            case set_pixel:
                            SubArray = SubArray | PixelHandler;
                            break;

            case reset_pixel:
                            SubArray = SubArray & ~PixelHandler;
                            break;

            case xor_pixel:
                            SubArray = SubArray ^ PixelHandler;
                            break;
            case rewrite_pixel:
                            SubArray = SubArray & ~PixelHandler;
                            SubArray = SubArray | PixelHandler;
                            break;
            default:
                    break;
        }
        ST7920_write_two_bytes(Column, Row, SubArray);
    }

}

/* Clear whole display in graphic mode */
void ST7920_clear_graphic(void)
{
    uint8_t Row, Column;
    for(Row = 0u; Row < ST7920_DISP_HEIGTH; Row++)
    {
        ST7920_set_addr_graphic(0u, Row);
        for(Column = 0u; Column < ST7920_PIX_IN_SUBARRAY; Column++)
        {
            ST7920_write_two_bytes(Column, Row, 0x0000);
        }
    }
}
/* Write two bytes(pixels) into display (graphic mode) on declared position */
/* Row <0-63>, Column <0-7> (Column=subarray)*/
void ST7920_write_two_bytes(uint8_t Column, uint8_t Row, uint16_t TwoBytes)
{
    uint8_t FirsByte = (TwoBytes >> 8) & 0xFF;
    uint8_t SecondByte = TwoBytes & 0xFF;

    ST7920_LCD_bit_array[Column][Row] = TwoBytes;
    ST7920_set_addr_graphic(Column, Row);
    ST7920_write_data(FirsByte);
    ST7920_write_data(SecondByte);
}

/* Set address in graphic mode - Row <0-63>, Column <0-7> (Column=subarray) */
void ST7920_set_addr_graphic(uint8_t Column, uint8_t Row)
{
    Row &= 0x3F;    /* max 63 rows */
    Column &= 0x7;  /* max 7 columns */
    if(Row < 32u)   /* the first half of the display */
    {
        ST7920_write_command(ST7920_SET_GDRAM_ADDR + Row);
        ST7920_write_command(ST7920_SET_GDRAM_ADDR + Column);
    }
    else /* the second half of the display */
    {
        ST7920_write_command(ST7920_SET_GDRAM_ADDR + Row - 32u);
        ST7920_write_command(ST7920_SET_GDRAM_ADDR + Column + 8u);
    }
}

/* initial setting of display - graphic mode*/
void ST7920_init(void)
{
    //HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
    //HAL_GPIO_WritePin(SCLK_PORT, SCLK_PIN, GPIO_PIN_RESET);
    //HAL_GPIO_WritePin(SID_PORT, SID_PIN, GPIO_PIN_RESET);


    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(45);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);

    ST7920_write_command(ST7920_FUNCTION_SET + ST7920_8BIT_BUS + ST7920_GRAPHIC_ON);  /* function set */
    HAL_Delay(1);
    ST7920_write_command(ST7920_FUNCTION_SET + ST7920_8BIT_BUS + ST7920_EX_INSTRUCTION + ST7920_GRAPHIC_ON);  /* function set */
    HAL_Delay(1);
    ST7920_write_command(ST7920_DISPLAY_CONTROL + ST7920_DISPLAY_ON);    /* display on/off control; display status */
    HAL_Delay(1);
    ST7920_write_command(ST7920_DISPLAY_CLEAR);                          /* display clear */
    HAL_Delay(15);
    ST7920_write_command(ST7920_ENTRY_MODE + ST7920_AC_INCREASE);        /* entry mode set */


}

/* Write command into display (serial mode) */
void ST7920_write_command(uint8_t Command)
{
    ST7920_parallel_write_byte(false, false, Command);
}

/* Write data into display (serial mode) */
void ST7920_write_data(uint8_t Data)
{
    ST7920_parallel_write_byte(false, true, Data);
}

/* serial communication driver */
/* send one byte into display */
/* RnW - true = Read, false = Write (Read is not supported in serial mode)*/
/* nID - true = Data, false = Instruction */
/* BitRegister - transfering byte of Instruction / Data */
void ST7920_parallel_write_byte(bool RnW, bool nID, uint8_t BitRegister)
{
	TimerDelay_us(100);

	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);

	if(RnW)
		HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET);

	if (nID)
		HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);

	/* -------------------------------------------------------------- */

	if(BitRegister & 0x80)
		HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB7_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB7_Pin, GPIO_PIN_RESET);

	if(BitRegister & 0x40)
		HAL_GPIO_WritePin(LCD_DB6_GPIO_Port, LCD_DB6_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB6_Pin, GPIO_PIN_RESET);

	if(BitRegister & 0x20)
		HAL_GPIO_WritePin(LCD_DB5_GPIO_Port, LCD_DB5_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_DB5_GPIO_Port, LCD_DB5_Pin, GPIO_PIN_RESET);

	if(BitRegister & 0x10)
		HAL_GPIO_WritePin(LCD_DB4_GPIO_Port, LCD_DB4_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_DB4_GPIO_Port, LCD_DB4_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET);
	/* -------------------------------------------------------------- */
	TimerDelay_us(100);

	if(BitRegister & 0x08)
		HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB7_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB7_Pin, GPIO_PIN_RESET);

	if(BitRegister & 0x04)
		HAL_GPIO_WritePin(LCD_DB6_GPIO_Port, LCD_DB6_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB6_Pin, GPIO_PIN_RESET);

	if(BitRegister & 0x02)
		HAL_GPIO_WritePin(LCD_DB5_GPIO_Port, LCD_DB5_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_DB5_GPIO_Port, LCD_DB5_Pin, GPIO_PIN_RESET);

	if(BitRegister & 0x01)
		HAL_GPIO_WritePin(LCD_DB4_GPIO_Port, LCD_DB4_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LCD_DB4_GPIO_Port, LCD_DB4_Pin, GPIO_PIN_RESET);

	/* -------------------------------------------------------------- */

	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);

}
