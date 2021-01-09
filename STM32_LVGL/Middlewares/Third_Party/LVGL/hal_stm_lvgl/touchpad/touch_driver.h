#ifndef TOUCH_DRIVER_H_
#define TOUCH_DRIVER_H_
#include "stm32h7xx_hal.h"
#include "ft5336.h"

void Touch_Init(void);
uint8_t Touch_Get_State(FT5336_State_t *TS_State);

#endif /* TOUCH_DRIVER_H_ */
