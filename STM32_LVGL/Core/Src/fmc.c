/**
  ******************************************************************************
  * File Name          : FMC.c
  * Description        : This file provides code for the configuration
  *                      of the FMC peripheral.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "fmc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SDRAM_HandleTypeDef hsdram1;

/* FMC initialization function */
void MX_FMC_Init(void)
{
  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_1;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_DISABLE;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 16;
  SdramTiming.ExitSelfRefreshDelay = 16;
  SdramTiming.SelfRefreshTime = 16;
  SdramTiming.RowCycleDelay = 16;
  SdramTiming.WriteRecoveryTime = 16;
  SdramTiming.RPDelay = 16;
  SdramTiming.RCDDelay = 16;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */

  /* USER CODE END FMC_Init 2 */
}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (FMC_Initialized) {
    return;
  }
  FMC_Initialized = 1;

  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();

  /** FMC GPIO Configuration
  PG15   ------> FMC_SDNCAS
  PD1   ------> FMC_D3
  PE0   ------> FMC_NBL0
  PE1   ------> FMC_NBL1
  PD0   ------> FMC_D2
  PG8   ------> FMC_SDCLK
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PG5   ------> FMC_BA1
  PF2   ------> FMC_A2
  PF4   ------> FMC_A4
  PG4   ------> FMC_BA0
  PF3   ------> FMC_A3
  PF5   ------> FMC_A5
  PD14   ------> FMC_D0
  PD15   ------> FMC_D1
  PE12   ------> FMC_D9
  PD8   ------> FMC_D13
  PD10   ------> FMC_D15
  PF13   ------> FMC_A7
  PE7   ------> FMC_D4
  PE13   ------> FMC_D10
  PH6   ------> FMC_SDNE1
  PF11   ------> FMC_SDNRAS
  PF15   ------> FMC_A9
  PE14   ------> FMC_D11
  PE10   ------> FMC_D7
  PD9   ------> FMC_D14
  PH5   ------> FMC_SDNWE
  PF14   ------> FMC_A8
  PG1   ------> FMC_A11
  PE9   ------> FMC_D6
  PE15   ------> FMC_D12
  PF12   ------> FMC_A6
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PE11   ------> FMC_D8
  PH7   ------> FMC_SDCKE1
  */
  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = SDNCAS_Pin|SDCLK_Pin|A15_Pin|A14_Pin
                          |A11_Pin|A10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = D3_Pin|D2_Pin|D0_Pin|D1_Pin
                          |D13_Pin|D15_Pin|D14_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = FMC_NBL0_Pin|FMC_NBL1_Pin|D9_Pin|D4_Pin
                          |D10_Pin|D11_Pin|D7_Pin|D6_Pin
                          |D12_Pin|D5_Pin|D8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = A1_Pin|A0_Pin|A2_Pin|A4_Pin
                          |A3_Pin|A5_Pin|A7_Pin|SDNRAS_Pin
                          |A9_Pin|A8_Pin|A6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = SDNE1_Pin|SDNWE_Pin|SDCKE1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* USER CODE BEGIN FMC_MspInit 1 */

  /* USER CODE END FMC_MspInit 1 */
}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* sdramHandle){
  /* USER CODE BEGIN SDRAM_MspInit 0 */

  /* USER CODE END SDRAM_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN SDRAM_MspInit 1 */

  /* USER CODE END SDRAM_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void){
  /* USER CODE BEGIN FMC_MspDeInit 0 */

  /* USER CODE END FMC_MspDeInit 0 */
  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_DISABLE();

  /** FMC GPIO Configuration
  PG15   ------> FMC_SDNCAS
  PD1   ------> FMC_D3
  PE0   ------> FMC_NBL0
  PE1   ------> FMC_NBL1
  PD0   ------> FMC_D2
  PG8   ------> FMC_SDCLK
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PG5   ------> FMC_BA1
  PF2   ------> FMC_A2
  PF4   ------> FMC_A4
  PG4   ------> FMC_BA0
  PF3   ------> FMC_A3
  PF5   ------> FMC_A5
  PD14   ------> FMC_D0
  PD15   ------> FMC_D1
  PE12   ------> FMC_D9
  PD8   ------> FMC_D13
  PD10   ------> FMC_D15
  PF13   ------> FMC_A7
  PE7   ------> FMC_D4
  PE13   ------> FMC_D10
  PH6   ------> FMC_SDNE1
  PF11   ------> FMC_SDNRAS
  PF15   ------> FMC_A9
  PE14   ------> FMC_D11
  PE10   ------> FMC_D7
  PD9   ------> FMC_D14
  PH5   ------> FMC_SDNWE
  PF14   ------> FMC_A8
  PG1   ------> FMC_A11
  PE9   ------> FMC_D6
  PE15   ------> FMC_D12
  PF12   ------> FMC_A6
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PE11   ------> FMC_D8
  PH7   ------> FMC_SDCKE1
  */

  HAL_GPIO_DeInit(GPIOG, SDNCAS_Pin|SDCLK_Pin|A15_Pin|A14_Pin
                          |A11_Pin|A10_Pin);

  HAL_GPIO_DeInit(GPIOD, D3_Pin|D2_Pin|D0_Pin|D1_Pin
                          |D13_Pin|D15_Pin|D14_Pin);

  HAL_GPIO_DeInit(GPIOE, FMC_NBL0_Pin|FMC_NBL1_Pin|D9_Pin|D4_Pin
                          |D10_Pin|D11_Pin|D7_Pin|D6_Pin
                          |D12_Pin|D5_Pin|D8_Pin);

  HAL_GPIO_DeInit(GPIOF, A1_Pin|A0_Pin|A2_Pin|A4_Pin
                          |A3_Pin|A5_Pin|A7_Pin|SDNRAS_Pin
                          |A9_Pin|A8_Pin|A6_Pin);

  HAL_GPIO_DeInit(GPIOH, SDNE1_Pin|SDNWE_Pin|SDCKE1_Pin);

  /* USER CODE BEGIN FMC_MspDeInit 1 */

  /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef* sdramHandle){
  /* USER CODE BEGIN SDRAM_MspDeInit 0 */

  /* USER CODE END SDRAM_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN SDRAM_MspDeInit 1 */

  /* USER CODE END SDRAM_MspDeInit 1 */
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
