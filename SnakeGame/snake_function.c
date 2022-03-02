/*
 * snake_function.c
 *
 *  Created on: Feb 27, 2022
 *      Author: 42077
 */

#include "snake_function.h"

/*
* Created by Vladimir Sustek 26.2.2022
* Use/copy/distribute me as you wish
* Bless you all
*/


/* Needed for printf*/
#include <stdio.h>

/* Needed for uints*/
#include <stdint.h>

/* Needed for SetConsoleCursorInfo and Sleep*/
/* Needed for getch - no*/

static uint32_t gPrgCycle = 0;

void snake_control(snake_t* snake)
{
	platform_get_control(snake);
}


void snake_hw_init(void)
{
	platform_init();
}

void snake_init(snake_t* snake)
{
	snake->length = SNAKE_INIT_LNG;
	snake->direction = PAUSE;
	snake->state = PLAYING;
	snake->ghost.x = INVALID_COORDS;
	snake->ghost.y = INVALID_COORDS;

	memset(&snake->body[0], 0, SNAKE_MAX_LNG*sizeof(coord_t));

	for (int idx = 0; idx < SNAKE_INIT_LNG; idx++)
	{
		snake->body[idx].x = SNAKE_INIT_X_CORD + idx;
		snake->body[idx].y = SNAKE_INIT_Y_CORD;
	}

	platform_refresh_hw();

}

void snake_display(snake_t* snake)
{
	if (INVALID_COORDS != snake->ghost.x && INVALID_COORDS != snake->ghost.y)
	{
		platform_eraseCell(snake->ghost.x, snake->ghost.y);
	}
	for (int idx = 0; idx < snake->length; idx++)
	{
		platform_drawCell(snake->body[idx].x, snake->body[idx].y);
	}
}


void snake_diplay_borders(void)
{
}

void snake_move(snake_t* snake)
{
	if (NULL == snake || PAUSE == snake->direction)
	{
		return;
	}
	snake->ghost = snake->body[0];
	memcpy(&snake->body[0], &snake->body[1], sizeof(coord_t) * (snake->length - 1));

	switch (snake->direction)
	{
	case UP:
	{
		if ((snake->body[snake->length - 1].y - 1) == ARENA_MIN_Y)
		{
			snake->state = CRASHED;
			break;
		}
		for (int idx = 0; idx < snake->length; idx++)
		{
			if (((snake->body[snake->length - 1].y - 1) == snake->body[idx].y) &&
				((snake->body[snake->length - 1].x) == snake->body[idx].x))
			{
				snake->state = CRASHED;
			}
		}
		snake->body[snake->length - 1].y--;
	}
	break;
	case DOWN:
	{
		if ((snake->body[snake->length - 1].y + 1) == ARENA_MAX_Y)
		{
			snake->state = CRASHED;
			break;
		}
		for (int idx = 0; idx < snake->length; idx++)
		{
			if (((snake->body[snake->length - 1].y + 1) == snake->body[idx].y) &&
				((snake->body[snake->length - 1].x) == snake->body[idx].x))
			{
				snake->state = CRASHED;
			}
		}

		snake->body[snake->length - 1].y++;
	}
	break;
	case RIGHT:
	{
		if ((snake->body[snake->length - 1].x + 1) == ARENA_MAX_X)
		{
			snake->state = CRASHED;
			break;
		}
		for (int idx = 0; idx < snake->length; idx++)
		{
			if (((snake->body[snake->length - 1].x + 1) == snake->body[idx].x) &&
				((snake->body[snake->length - 1].y) == snake->body[idx].y))
			{
				snake->state = CRASHED;
			}
		}
		snake->body[snake->length - 1].x++;
	}
	break;
	case LEFT:
	{
		if ((snake->body[snake->length - 1].x - 1) == ARENA_MIN_X)
		{
			snake->state = CRASHED;
			break;
		}
		for (int idx = 0; idx < snake->length; idx++)
		{
			if (((snake->body[snake->length - 1].x - 1) == snake->body[idx].x) &&
				((snake->body[snake->length - 1].y) == snake->body[idx].y))
			{
				snake->state = CRASHED;
			}
		}
		snake->body[snake->length - 1].x--;
	}
	break;
	default:
	{
	}
	}

	if (snake->length == SNAKE_WON_LIMIT)
	{
		snake->state = WON;
	}
}

uint16_t generate_food(snake_t* snake, food_t *food)
{
	uint16_t isInvalid = 0;
	uint16_t iter = 0;

	do
	{
		food->coord.x = (uint16_t)((platform_randomize() % (FOOD_MAX_X - FOOD_MIN_X + 1)) + FOOD_MIN_X);
		food->coord.y = (uint16_t)((platform_randomize() % (FOOD_MAX_Y - FOOD_MIN_Y + 1)) + FOOD_MIN_Y);

		for (int idx = 0; idx < snake->length; idx++)
		{
			if ((snake->body[idx].x == food->coord.x) &&
				(snake->body[idx].y == food->coord.y))
			{
				isInvalid = GENERAL_ERROR;
				break;
			}
		}
		if (iter > FOOD_MAX_ITER)
		{
			break;
		}

	} while (isInvalid);

	if (!isInvalid)
	{
		platform_drawFood(food->coord.x, food->coord.y);
	}

	return isInvalid;

}


void snake_place_food(snake_t* snake, food_t* food, uint32_t tick)
{
	if (0 == gPrgCycle % 10 || food->time_elapsed)
	{
		if (food->state != PLACED)
		{
			if (GENERAL_ERROR == generate_food(snake, food))
			{
				platform_fatal();
			}
			else
			{
				food->time_elapsed = 0;
				food->state = PLACED;
			}
		}
		else
		{
			food->time_elapsed = 1;
		}

	}

}
void snake_haseaten(snake_t* snake, food_t* food)
{


	if ((snake->body[snake->length - 1].x == food->coord.x)
		&& (snake->body[snake->length - 1].y == food->coord.y))
	{
		/* Needed temporary copy for shifting the whole array right - for embedded*/
		coord_t tempSnake[SNAKE_MAX_LNG] = {0};
		memcpy(tempSnake, &(snake->body[0]), (size_t)snake->length*sizeof(coord_t));

		/* Just append the ghost to the end, increment length and disable ghost*/
		memcpy(&(snake->body[1]), tempSnake, (size_t)snake->length*sizeof(coord_t));
		snake->body[0] = snake->ghost;
		snake->ghost.x = INVALID_COORDS;
		snake->ghost.y = INVALID_COORDS;
		snake->length++;

		food->state = EATEN;
	}
}

void snake_inform(snake_t* snake)
{

}
