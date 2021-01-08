#include "encode_hw.h"

uint32_t JPEG_Size;

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

  uint32_t ybcr_mcu= 0;
  uint32_t ybcr_size= 0;

  Conf.ImageWidth         = img_x;
  Conf.ImageHeight        = img_y;
  Conf.ChromaSubsampling  = JPEG_420_SUBSAMPLING;
  Conf.ColorSpace         = JPEG_YCBCR_COLORSPACE;
  Conf.ImageQuality       = 75;

  JPEG_GetEncodeColorConvertFunc(&Conf, &pRGBToYCbCr_Convert_Function, &ybcr_mcu);

  pRGBToYCbCr_Convert_Function(img, img_tmp, 0, (img_x*img_y*img_bytpp), &ybcr_size);

  HAL_JPEG_ConfigEncoding(hjpeg, &Conf);

  HAL_JPEG_Encode(hjpeg, img_tmp ,ybcr_size ,jpg_out, (img_x*img_y*img_bytpp), 1000);

  return JPEG_Size;
}

/**
  * @brief JPEG Ouput Data BackGround processing .
  * @param hjpeg: JPEG handle pointer
  * @retval 1 : if JPEG processing has finiched, 0 : if JPEG processing still ongoing
  */
uint32_t JPEG_EncodeOutputHandler(JPEG_HandleTypeDef *hjpeg)
{
  return 0;  
}

/**
  * @brief JPEG Input Data BackGround Preprocessing .
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void JPEG_EncodeInputHandler(JPEG_HandleTypeDef *hjpeg)
{

}

/**
  * @brief JPEG Get Data callback
  * @param hjpeg: JPEG handle pointer
  * @param NbEncodedData: Number of encoded (consummed) bytes from input buffer
  * @retval None
  */
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbEncodedData)
{
	HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_INPUT);
}

/**
  * @brief JPEG Data Ready callback
  * @param hjpeg: JPEG handle pointer
  * @param pDataOut: pointer to the output data buffer
  * @param OutDataLength: length of output buffer in bytes
  * @retval None
  */
void HAL_JPEG_DataReadyCallback (JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
  HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_OUTPUT);
  JPEG_Size = OutDataLength;
}

/**
  * @brief  JPEG Error callback
  * @param hjpeg: JPEG handle pointer
  * @retval None
  */
void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg)
{
	while(1)
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
}
