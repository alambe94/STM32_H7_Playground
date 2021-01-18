#include "cmsis_os.h"

#include "jpeg.h"
#include "jpeg_encode.h"

#define JPEG_THREAD_STACK_SIZE 1024
#define JPEG_THREAD_STACK_PRIO 4
StackType_t JPEG_Thread_Stack[JPEG_THREAD_STACK_SIZE];
StaticTask_t JPEG_Thread_TCB;
TaskHandle_t hJPEG_Thread;

uint8_t img_tmp[480 * 272 * 2];

extern void *LCD_Get_Frame_Buffer(void);
extern void *UVC_Get_Frame_Buffer(uint32_t *size);
extern void UVC_Set_Event(uint32_t size, uint8_t encoded_flag);
extern uint8_t UVC_Get_Event(void);

void JPEG_Thread(void *argument);

void JPEG_Thread_Add(void)
{
	hJPEG_Thread = xTaskCreateStatic(JPEG_Thread,
									 "JPEG",
									 JPEG_THREAD_STACK_SIZE,
									 NULL,
									 JPEG_THREAD_STACK_PRIO,
									 JPEG_Thread_Stack,
									 &JPEG_Thread_TCB);
}

void JPEG_Thread(void *argument)
{
	JPEG_InitColorTables();

	while (1)
	{
		if (1)
		{
			uint32_t out_jpj_sz = 0x00;
			uint32_t jpj_sz = 0x00;

			uint8_t *in_jpj = LCD_Get_Frame_Buffer();
			uint8_t *out_jpj = UVC_Get_Frame_Buffer(&out_jpj_sz);

			uint32_t tick = HAL_GetTick();

			extern uint32_t Image_RGB888[];
			jpj_sz = JPEG_Encode_SW((uint8_t*)Image_RGB888, 320, 240, 3, 75, out_jpj, out_jpj_sz);


//			jpj_sz = JPEG_Encode_HW(&hjpeg,
//									in_jpj,
//									480,
//									272,
//									2,
//									75,
//									img_tmp,
//									out_jpj);

//			JPEG_Encode_HW_DMA(&hjpeg,
//							   in_jpj,
//							   480,
//							   272,
//							   2,
//							   75,
//							   img_tmp,
//							   out_jpj);
//
//			while (!JPEG_Get_Status(&jpj_sz))
//			{
//				vTaskDelay(5);
//			}

			UVC_Set_Event(jpj_sz, 1);

			tick = HAL_GetTick() - tick;
			tick++;
		}
		else
		{
			vTaskDelay(10);
		}
	}
}
