#include "encode_hw.h"

uint32_t JPEG_Size;
uint32_t YCBCR_Size;
uint32_t YCBCR_Index;
uint32_t YCBCR_Packets;
uint32_t YCBCR_Packet_Size;
uint8_t *YCBCR_IMG;
uint8_t DMA_Mode;

uint8_t Encoded_Flag;

uint32_t JPEG_Encode_HW(JPEG_HandleTypeDef *hjpeg,
                        uint8_t *img,
                        uint32_t img_x,
                        uint32_t img_y,
                        uint8_t img_bytpp,
                        uint8_t *img_tmp,
                        uint8_t *jpg_out)
{
  JPEG_ConfTypeDef Conf;
  JPEG_RGBToYCbCr_Convert_Function pRGBToYCbCr_Convert_Function;

  uint32_t ycbcr_mcu = 0;
  uint32_t ycbcr_size = 0;
  JPEG_Size = 0;
  DMA_Mode = 0;
  Encoded_Flag = 0;

  Conf.ImageWidth = img_x;
  Conf.ImageHeight = img_y;
  Conf.ChromaSubsampling = JPEG_420_SUBSAMPLING;
  Conf.ColorSpace = JPEG_YCBCR_COLORSPACE;
  Conf.ImageQuality = 75;

  JPEG_GetEncodeColorConvertFunc(&Conf, &pRGBToYCbCr_Convert_Function, &ycbcr_mcu);

  pRGBToYCbCr_Convert_Function(img, img_tmp, 0, (img_x * img_y * img_bytpp), &ycbcr_size);

  HAL_JPEG_ConfigEncoding(hjpeg, &Conf);

  HAL_JPEG_Encode(hjpeg, img_tmp, ycbcr_size, jpg_out, (img_x * img_y * img_bytpp), 1000);

  return JPEG_Size;
}

void JPEG_Encode_HW_DMA(JPEG_HandleTypeDef *hjpeg,
                            uint8_t *img,
                            uint32_t img_x,
                            uint32_t img_y,
                            uint8_t img_bytpp,
                            uint8_t *img_tmp,
                            uint8_t *jpg_out)
{
  JPEG_ConfTypeDef Conf;
  JPEG_RGBToYCbCr_Convert_Function pRGBToYCbCr_Convert_Function;

  uint32_t ycbcr_mcu = 0;
  uint32_t ycbcr_size = 0;
  JPEG_Size = 0;
  DMA_Mode = 1;
  Encoded_Flag = 0;

  Conf.ImageWidth = img_x;
  Conf.ImageHeight = img_y;
  Conf.ChromaSubsampling = JPEG_420_SUBSAMPLING;
  Conf.ColorSpace = JPEG_YCBCR_COLORSPACE;
  Conf.ImageQuality = 75;

  JPEG_GetEncodeColorConvertFunc(&Conf, &pRGBToYCbCr_Convert_Function, &ycbcr_mcu);

  pRGBToYCbCr_Convert_Function(img, img_tmp, 0, (img_x * img_y * img_bytpp), &ycbcr_size);

  HAL_JPEG_ConfigEncoding(hjpeg, &Conf);

  YCBCR_Index = 0x00;
  YCBCR_Size = ycbcr_size;
  YCBCR_IMG = img_tmp;
  YCBCR_Packet_Size = 8*1024;
  YCBCR_Packets = YCBCR_Size/YCBCR_Packet_Size;
  YCBCR_Index++;
  YCBCR_IMG += YCBCR_Packet_Size;
  HAL_JPEG_Encode_DMA(hjpeg, img_tmp, YCBCR_Packet_Size, jpg_out, 16*1024);
}

/**
  * @brief JPEG Get Data callback
  * @param hjpeg: JPEG handle pointer
  * @param NbEncodedData: Number of encoded (consummed) bytes from input buffer
  * @retval None
  */
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbEncodedData)
{
	if(DMA_Mode)
	{
		if(YCBCR_Index == YCBCR_Packets)
		{
			YCBCR_Packet_Size = YCBCR_Size%YCBCR_Packet_Size;
		}

		HAL_JPEG_ConfigInputBuffer(hjpeg, YCBCR_IMG, YCBCR_Packet_Size);
		YCBCR_Index++;
		YCBCR_IMG += YCBCR_Packet_Size;

		if(YCBCR_Index > YCBCR_Packets)
		{
			YCBCR_Index = 0x00;
			HAL_JPEG_ConfigInputBuffer(hjpeg, YCBCR_IMG, 4);
			// if HAL_JPEG_Pause here HAL_JPEG_DataReadyCallbackn will not be called
		}
	}
}

/**
  * @brief JPEG Data Ready callback
  * @param hjpeg: JPEG handle pointer
  * @param pDataOut: pointer to the output data buffer
  * @param OutDataLength: length of output buffer in bytes
  * @retval None
  */
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
  JPEG_Size = OutDataLength;
}

/**
  * @brief  JPEG Error callback
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg)
{
  while (1)
  {
  }
}

/*
  * @brief JPEG Decode complete callback
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void HAL_JPEG_EncodeCpltCallback(JPEG_HandleTypeDef *hjpeg)
{
	Encoded_Flag = 1;
}

uint8_t JPEG_Get_Status(uint32_t *size)
{
	uint8_t tmp = Encoded_Flag;
	*size = JPEG_Size;
	Encoded_Flag = 0;
	return tmp;
}
