#include "tx_api.h"
#include "main.h"
#include "stm32h7xx_hal.h"

TX_BYTE_POOL BytePool;
#define BYTE_POOL_SIZE (12 * 1024)

#define MAIN_STACK_SIZE 512
#define MAIN_THREAD_PRIO 5
TX_THREAD Main_Thread;
void Main_Thread_Entry(ULONG thread_input);

void tx_application_define(void *memory_ptr)
{
	UINT status = TX_SUCCESS;
	CHAR *pointer;

	/* Create a byte memory pool from which to allocate the thread stacks.  */
	if (tx_byte_pool_create(&BytePool, "Byte Pool", memory_ptr,
							BYTE_POOL_SIZE) != TX_SUCCESS)
	{
		status = TX_POOL_ERROR;
	}

	/* Allocate the stack for MainThread.  */
	if (tx_byte_allocate(&BytePool, (VOID **)&pointer,
						 MAIN_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
	{
		status = TX_POOL_ERROR;
	}

	/* Create MainThread.  */
	if (tx_thread_create(&Main_Thread, "Main Thread", Main_Thread_Entry, 0,
						 pointer, MAIN_STACK_SIZE,
						 MAIN_THREAD_PRIO, MAIN_THREAD_PRIO,
						 TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
	{
		status = TX_THREAD_ERROR;
	}

	(void)status;
}

void Main_Thread_Entry(ULONG thread_input)
{
	while (1)
	{
		HAL_GPIO_TogglePin(USER_LED1_GPIO_Port, USER_LED1_Pin);
		tx_thread_sleep(1000);
	}
}
