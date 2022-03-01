/*
 * habitat_dbgcom.h
 *
 *  Created on: Mar 12, 2019
 *      Author: Vladimir Sustek
 */

#ifndef HABITAT_DBGCOM_H_
#define HABITAT_DBGCOM_H_

#include "usart.h"
#include "string.h"

uint32_t uart3_listen_start(void);
int uart3_check_dma(void);

extern char extKeyBoardButton;

#endif /* HABITAT_DBGCOM_H_ */
