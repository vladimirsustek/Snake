/*
 * habitat_dbgcom.c
 *
 *  Created on: Mar 12, 2019
 *      Author: Vladimir Sustek
 */

#include "uart_com.h"

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

#define UART_BUFFER_SIZE 	(uint32_t)(1024)

#define uart3_rx_write_ptr (UART_BUFFER_SIZE - hdma_usart3_rx.Instance->NDTR)

static char uart3_rx_buf[UART_BUFFER_SIZE];
static uint32_t uart3_rx_read_ptr = 0;

static char* uart3_store_received(char recvd);

/* Start UART DMA RX mode */
uint32_t uart3_listen_start(void)
{

	HAL_StatusTypeDef result = 0;

	result = HAL_UART_Receive_DMA(&huart3, (uint8_t*) uart3_rx_buf, UART_BUFFER_SIZE);

	return result;
}


/* Store received byte (char) and in case of detected \n, toggle LED */
char* uart3_store_received(char recvd)
{

	char* p_rcvd = NULL;
	static char ring_buffer_uart3[UART_BUFFER_SIZE];
	static uint32_t idx_uart3 = 0;

	ring_buffer_uart3[idx_uart3] = recvd;
	ring_buffer_uart3[(idx_uart3 + 1) & (UART_BUFFER_SIZE - 1)] = '\0';

	idx_uart3 = (idx_uart3 + 1) & (UART_BUFFER_SIZE - 1);

	if (recvd == '\n')
	{

		if (memcmp(ring_buffer_uart3, "ACTION", 4) == 0)
		{
			/*Action to do*/
		}
		idx_uart3 = 0;
		ring_buffer_uart3[(idx_uart3 + 1) & (UART_BUFFER_SIZE - 1)] = '\0';
	}

	return p_rcvd;
}

/* Checks content of DMA RX uart buffer and eventually stores new bytes
 * using the uart2_store_received - declared upper*/
int uart3_check_dma(void)
{

	int result = 0;
	char rec_c = '\0';

	while (uart3_rx_read_ptr != uart3_rx_write_ptr)
	{
		__ISB();
		//rec_c = uart3_rx_buf[uart3_rx_read_ptr];
		extKeyBoardButton = uart3_rx_buf[uart3_rx_read_ptr];
		if (++uart3_rx_read_ptr >= UART_BUFFER_SIZE)
		{
			uart3_rx_read_ptr = 0;
		}
		//uart3_store_received(rec_c);
	}
	return result;
}


/* To be used further */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{

	}
}

/* UART Redirection for user purposes*/
int _write(int file, char const *buf, int n)
{
    /* stdout redirection to UART3 */
	HAL_UART_Transmit(&huart3, (uint8_t*)(buf), n, HAL_MAX_DELAY);
	return n;
}


