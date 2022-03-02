/*
 * port.h
 *
 *  Created on: Feb 27, 2022
 *      Author: 42077
 */

#ifndef SNAKE_PORT_H_
#define SNAKE_PORT_H_

#include <stdint.h>
#include <string.h>

/* LCD dependecies */
#include "tft.h"
#include "functions.h"
/* Randomizer dependencies */
#include "adc.h"
#include "stm32f7xx_hal.h"
#include "usart.h"

#define ARENA_OFFSET_X		(uint16_t)(6)
#define ARENA_OFFSET_Y		(uint16_t)(9)
#define CELL_SIZE			(uint16_t)(22)

#define ARENA_MAX_X			(uint16_t)(14)
#define ARENA_MAX_Y			(uint16_t)(21)
#define ARENA_MIN_X			(uint16_t)(0)
#define ARENA_MIN_Y			(uint16_t)(0)

#define FOOD_MAX_X			(uint16_t)(13)
#define FOOD_MIN_X			(uint16_t)(1)
#define FOOD_MAX_Y			(uint16_t)(20)
#define FOOD_MIN_Y			(uint16_t)(1)

#define SNAKE_MAX_LNG		(uint16_t)(100)
#define SNAKE_INIT_LNG		(uint16_t)(3)
#define SNAKE_WON_LIMIT		(uint16_t)(SNAKE_MAX_LNG - 1)

#define SNAKE_INIT_X_CORD	(uint16_t)(1)
#define SNAKE_INIT_Y_CORD	(uint16_t)(10)

#define SNAKE_MOVE_OK		(uint16_t)(0)
#define SNAKE_MOVE_CRASH	(uint16_t)(1)
#define SNAKE_WON			(uint16_t)(2)

#define FOOD_MAX_ITER		(uint16_t)(100)

#define GENERAL_ERROR		(uint16_t)(-1)
#define INVALID_COORDS		(uint16_t)(-1)

typedef enum { UP = 'W', DOWN = 'S', LEFT =  'A', RIGHT = 'D', PAUSE = 'P', QUIT = 'Q' } snake_dir_e;

typedef enum { WAITING, PLACED, EATEN } foodstate_e;

typedef enum { PLAYING, CRASHED, WON } snake_state_e;

typedef struct coord_tag
{
	uint16_t x;
	uint16_t y;
} coord_t;
;
typedef struct snake_tag
{
	snake_dir_e direction;
	coord_t body[SNAKE_MAX_LNG];
	uint16_t length;
	coord_t ghost;
	snake_state_e state;
} snake_t;

typedef struct food_tag
{
	coord_t coord;
	foodstate_e state;
	uint16_t time_elapsed;
} food_t;

void platform_drawCell(uint16_t x, uint16_t y);
void platform_eraseCell(uint16_t x, uint16_t y);
void platform_drawFood(uint16_t x, uint16_t y);
void platform_eraseFood(uint16_t x, uint16_t y);

void platform_init(void);
void platform_printXY(uint16_t x, uint16_t y, char symbol);
uint16_t platform_randomize(void);
void platform_sleep(uint16_t ms);
void platform_showInformal(char* str, uint16_t length);
void platform_fatal(void);
void platform_get_control(snake_t* snake);
void platform_refresh_hw(void);

#endif /* SNAKE_PORT_H_ */
