#include "i2c.h"
#include "touch_driver.h"

FT5336_IO_t Touch_IO;
FT5336_Object_t hTouch;

int32_t FT5336_IO_Init(void);
int32_t FT5336_IO_DeInit(void);
int32_t FT5336_IO_GetTick(void);
int32_t FT5336_IO_Delay(uint32_t);
int32_t FT5336_IO_WriteReg(uint16_t, uint16_t, uint8_t *, uint16_t);
int32_t FT5336_IO_ReadReg(uint16_t, uint16_t, uint8_t *, uint16_t);

void Touch_Init(void)
{
  uint32_t ft5336_id = 0;

  Touch_IO.Address = 0x70U;
  Touch_IO.Init = FT5336_IO_Init;
  Touch_IO.DeInit = FT5336_IO_DeInit;
  Touch_IO.ReadReg = FT5336_IO_ReadReg;
  Touch_IO.WriteReg = FT5336_IO_WriteReg;
  Touch_IO.GetTick = FT5336_IO_GetTick;

  FT5336_RegisterBusIO(&hTouch, &Touch_IO);

  FT5336_ReadID(&hTouch, &ft5336_id);

  if (ft5336_id != FT5336_ID)
  {
	  //error
  }
}

uint8_t Touch_Get_State(FT5336_State_t *TS_State)
{
  static uint32_t prevx;
  static uint32_t prevy;

  uint32_t x_oriented, y_oriented;
  uint32_t x_diff, y_diff;

  FT5336_State_t state;

  FT5336_GetState(&hTouch, &state);

  if (state.TouchDetected != 0U)
  {
    x_oriented = state.TouchX;
    y_oriented = state.TouchY;

    // swap x and y
    x_oriented = state.TouchY;
    y_oriented = state.TouchX;

    TS_State->TouchX = x_oriented;
    TS_State->TouchY = y_oriented;

    /* Store Current TS state */
    TS_State->TouchDetected = state.TouchDetected;

    /* Check accuracy */
    x_diff = (TS_State->TouchX > prevx) ? (TS_State->TouchX - prevx) : (prevx - TS_State->TouchX);

    y_diff = (TS_State->TouchY > prevy) ? (TS_State->TouchY - prevy) : (prevy - TS_State->TouchY);

    if (x_diff > 8 || y_diff > 8)
    {
      /* New touch detected */
      prevx = TS_State->TouchX;
      prevy = TS_State->TouchY;
    }
    else
    {
      TS_State->TouchX = prevx;
      TS_State->TouchY = prevy;
    }
  }
  else
  {
    TS_State->TouchDetected = 0U;
    TS_State->TouchX = prevx;
    TS_State->TouchY = prevy;
  }

  return 1;
}


int32_t FT5336_IO_Init(void)
{
	// I2C and GPIO configured in cube
	return FT5336_OK;
}

int32_t FT5336_IO_DeInit(void)
{
	return FT5336_OK;
}

int32_t FT5336_IO_GetTick(void)
{
	return HAL_GetTick();
}

int32_t FT5336_IO_Delay(uint32_t delay)
{
	HAL_Delay(delay);
	return FT5336_OK;
}

int32_t FT5336_IO_WriteReg(uint16_t add, uint16_t reg, uint8_t *data, uint16_t len)
{
	HAL_I2C_Mem_Write(&hi2c4, 0x70U, reg, 1, data, len, 100);
	return FT5336_OK;
}

int32_t FT5336_IO_ReadReg(uint16_t add, uint16_t reg, uint8_t *data, uint16_t len)
{
	HAL_I2C_Mem_Read(&hi2c4, 0x70U, reg, 1, data, len, 100);
	return FT5336_OK;
}
