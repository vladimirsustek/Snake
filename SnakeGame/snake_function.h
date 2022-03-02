/*
 * snake_function.h
 *
 *  Created on: Feb 27, 2022
 *      Author: 42077
 */

#ifndef SNAKE_FUNCTION_H_
#define SNAKE_FUNCTION_H_

#include "snake_port.h"

void snake_hw_init(void);
void snake_init(snake_t* snake);
void snake_display(snake_t* snake);
void snake_diplay_borders(void);
void snake_move(snake_t* snake);
uint16_t generate_food(snake_t* snake, food_t *food);
void snake_place_food(snake_t* snake, food_t* food, uint32_t tick);
void snake_haseaten(snake_t* snake, food_t* food);
void snake_inform(snake_t* snake);
void snake_control(snake_t* snake);

#endif /* SNAKE_FUNCTION_H_ */
