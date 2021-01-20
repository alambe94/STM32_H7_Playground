#include "cmsis_os.h"

#include "lvgl/lvgl.h"
#include "hal_stm_lvgl/tft/tft.h"
#include "hal_stm_lvgl/touchpad/touchpad.h"
#include "lv_examples/lv_examples.h"

#define LVGL_THREAD_STACK_SIZE 2048
#define LVGL_THREAD_STACK_PRIO 5
StackType_t LVGL_Thread_Stack[LVGL_THREAD_STACK_SIZE];
StaticTask_t LVGL_Thread_TCB;
TaskHandle_t hLVGL_Thread;

void LVGL_Thread(void *argument);

void LVGL_Thread_Add(void)
{
	hLVGL_Thread = xTaskCreateStatic(LVGL_Thread,
			                         "LVGL",
									 LVGL_THREAD_STACK_SIZE,
									 NULL,
									 LVGL_THREAD_STACK_PRIO,
									 LVGL_Thread_Stack,
									 &LVGL_Thread_TCB);
}

void LVGL_Thread(void *argument)
{
	lv_init();

	tft_init();
	touchpad_init();

	//lv_demo_music();
	//lv_demo_stress();
	lv_demo_widgets();
	//lv_demo_benchmark();
	//lv_demo_keypad_encoder();
	//lv_demo_printer();

	while(1)
	{
		lv_task_handler();
		vTaskDelay(16);
	}
}
