#include "cmsis_os.h"

#include "usbd_cdc_rndis_if.h"
#include "lwip/netif.h"

#define RNDIS_THREAD_STACK_SIZE 512
#define RNDIS_THREAD_STACK_PRIO 5
StackType_t RNDIS_Thread_Stack[RNDIS_THREAD_STACK_SIZE];
StaticTask_t RNDIS_Thread_TCB;
TaskHandle_t hRNDIS_Thread;

struct netif gnetif;

void RNDIS_Thread(void *argument);

void RNDIS_Thread_Add(void)
{
	hRNDIS_Thread = xTaskCreateStatic(RNDIS_Thread,
									 "RNDIS",
									 RNDIS_THREAD_STACK_SIZE,
									 NULL,
									 RNDIS_THREAD_STACK_PRIO,
									 RNDIS_Thread_Stack,
									 &RNDIS_Thread_TCB);
}

void RNDIS_Thread(void *argument)
{
	while (1)
	{
		extern USBD_HandleTypeDef  hUsbDeviceHS;
		USBD_CDC_RNDIS_fops.Process(&hUsbDeviceHS);
		vTaskDelay(10);
	}
}
