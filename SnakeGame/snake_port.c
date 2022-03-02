/*
 * port.c
 *
 *  Created on: Feb 27, 2022
 *      Author: 42077
 */


#include "snake_port.h"

/* Randomizer properties */
ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart3;

static uint16_t gRandSeed;

char extKeyBoardButton;
UART_HandleTypeDef huart3;
char sUartChar = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		extKeyBoardButton = sUartChar;
		sUartChar = 0;
		HAL_UART_Receive_IT(&huart3, (uint8_t*)&sUartChar, 1);
	}
}

static void platform_control_init(void)
{
	HAL_UART_Receive_IT(&huart3, (uint8_t*)&sUartChar, 1);
}
static void platform_init_randomizer(void)
{
	/* This randomizer is based on ADC noise
	 * as a LFSR seed number */
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	gRandSeed = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	while(gRandSeed < 0x8000) gRandSeed += gRandSeed;
}

static void platform_display_init(void)
{
    tft_init(readID());
}


void platform_refresh_hw(void)
{
    fillScreen(BLACK);
}

void platform_drawCell(uint16_t x, uint16_t y)
{

	drawRect(ARENA_OFFSET_X + CELL_SIZE*x,
			ARENA_OFFSET_Y + CELL_SIZE*y,
			CELL_SIZE,
			CELL_SIZE,
			WHITE);

	fillRect(ARENA_OFFSET_X + CELL_SIZE*x + 1,
			ARENA_OFFSET_Y + CELL_SIZE*y + 1,
			CELL_SIZE - 2,
			CELL_SIZE - 2,
			MAGENTA);
}

void platform_eraseCell(uint16_t x, uint16_t y)
{
	fillRect(ARENA_OFFSET_X + CELL_SIZE*x,
			ARENA_OFFSET_Y + CELL_SIZE*y,
			CELL_SIZE,
			CELL_SIZE,
			BLACK);
}

void platform_drawFood(uint16_t x, uint16_t y)
{
	fillCircle(ARENA_OFFSET_X + CELL_SIZE*x + CELL_SIZE/2,
			   ARENA_OFFSET_Y + CELL_SIZE*y + CELL_SIZE/2,
			   CELL_SIZE/3,
			   GREEN);
}

void platform_eraseFood(uint16_t x, uint16_t y)
{
	fillCircle(ARENA_OFFSET_X + CELL_SIZE*x + CELL_SIZE/2,
			   ARENA_OFFSET_Y + CELL_SIZE*y + CELL_SIZE/2,
			   CELL_SIZE/3,
			   BLACK);
}

void platform_init(void)
{
	platform_init_randomizer();
	platform_control_init();
	platform_display_init();
}

void platform_showInformal(char* str, uint16_t length)
{
}

uint16_t platform_randomize(void)
{
  uint16_t lsb;

  lsb = gRandSeed & 1;
  gRandSeed >>= 1;
  if (lsb == 1)
  {
	  gRandSeed ^= 0xB400u;
  }

  return gRandSeed;
}

void platform_sleep(uint16_t ms)
{
	return;
}

void platform_fatal(void)
{

	platform_showInformal("FatalError\n", strlen("FatalError\n"));
	while (1);
}

void platform_get_control(snake_t * snake)
{
	snake_dir_e direction = 0;
	static snake_dir_e prev_direction = RIGHT;

	direction = (snake_dir_e)extKeyBoardButton;

	if (direction == 0)
	{
		return;
	}

	extKeyBoardButton = 0;

	if ((direction != LEFT) && (direction != RIGHT) && (direction != UP) &&
		(direction != DOWN) && (direction != PAUSE) && (direction != QUIT))
	{
		prev_direction = snake->direction;
		snake->direction = PAUSE;
	}
	else
	{
		if (direction == PAUSE)
		{
			if (snake->direction != PAUSE)
			{
				prev_direction = snake->direction;
				snake->direction = PAUSE;
			}
			else
			{
				snake->direction = prev_direction;
			}
		}

		else
		{
			if ((snake->direction != PAUSE) &&
				!(snake->direction == LEFT && direction == RIGHT) &&
				!(snake->direction == RIGHT && direction == LEFT) &&
				!(snake->direction == UP && direction == DOWN) &&
				!(snake->direction == DOWN && direction == UP))
			{
				snake->direction = direction;
			}
		}
	}
}

