/*
 * port.c
 *
 *  Created on: Feb 27, 2022
 *      Author: 42077
 */


#include <snake_port.h>

/* Randomizer properties */
ADC_HandleTypeDef hadc1;
static uint16_t gRandSeed;


char extKeyBoardButton = 0;

void platform_drawCell(uint16_t x, uint16_t y)
{
	fillRect(ARENA_OFFSET_X + CELL_SIZE*x,
			ARENA_OFFSET_Y + CELL_SIZE*y,
			CELL_SIZE,
			CELL_SIZE,
			WHITE);
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
	drawCircle(ARENA_OFFSET_X + CELL_SIZE*x + CELL_SIZE/2,
			   ARENA_OFFSET_Y + CELL_SIZE*y + CELL_SIZE/2,
			   CELL_SIZE/2,
			   MAGENTA);
}

void platform_eraseFood(uint16_t x, uint16_t y)
{
	drawCircle(ARENA_OFFSET_X + CELL_SIZE*x + CELL_SIZE/2,
			   ARENA_OFFSET_Y + CELL_SIZE*y + CELL_SIZE/2,
			   CELL_SIZE/2,
			   BLACK);
}

void platform_init(void)
{
	/* Init randomizer */
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	gRandSeed = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	while(gRandSeed < 0x8000) gRandSeed += gRandSeed;

	uart3_listen_start();
    tft_init(readID());
}

void platform_printXY(uint16_t x, uint16_t y, char symbol)
{
	//COORD c;
	//c.X = (short)x;
	//c.Y = (short)y;
	//SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	//printf("%c", symbol);

}

void platform_showInformal(char* str, uint16_t length)
{
	platform_printXY(0, 0, '>');

	for (int idx = 0; idx < length; idx++)
	{
		//printf("%c", str[idx]);
	}
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
	HAL_Delay(ms);
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

	uart3_check_dma();
	direction = (snake_dir_e)extKeyBoardButton;

	if (direction == 0)
	{
		return;
	}

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


uint16_t platform_restart_game(void)
{
	if(HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin))
	{

	}
}
