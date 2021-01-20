#include "jpeg_encode.h"

#include "jpeglib.h"
#include "jdata_conf.h"

// variables used for dma mode
uint32_t YCBCR_Size;
uint32_t YCBCR_Index;
uint32_t YCBCR_Packets;
uint32_t YCBCR_Packet_Size;
uint8_t *YCBCR_IMG;
uint8_t Encoded_Flag;
uint8_t DMA_Mode;

uint32_t JPEG_Size;

uint32_t JPEG_Encode_SW(uint8_t *img,        // pointer input rgb map
                        uint32_t img_x,      // image width
                        uint32_t img_y,      // image hight
                        uint8_t img_bytpp,   // bytes per pixel
                        uint8_t quality,     // out jpeg quality
                        uint8_t *jpg_out,    // where out jpeg is stored, must be sizes appropriately
                        uint32_t jpg_out_sz) // size of output buffer
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  JSAMPROW row_pointer[1];
  uint32_t row_stride;

  cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_compress(&cinfo);
  cinfo.image_width = img_x;
  cinfo.image_height = img_y;

  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);

  jpeg_mem_dest(&cinfo, &jpg_out, &jpg_out_sz);

  jpeg_start_compress(&cinfo, TRUE);

  row_stride = img_x * img_bytpp;

  while (cinfo.next_scanline < cinfo.image_height)
  {
    row_pointer[0] = &img[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  return jpg_out_sz;
}

uint32_t JPEG_Encode_HW(JPEG_HandleTypeDef *hjpeg,
                        uint8_t *img,       // pointer input rgb map
                        uint32_t img_x,     // image width
                        uint32_t img_y,     // image hight
                        uint8_t img_bytpp,  // bytes per pixel
                        uint8_t quality,    // out jpeg quality
                        uint8_t *ycbcr_tmp, // temperory buffer for ycbcr conversion, must be sizes appropriately
                        uint8_t *jpg_out)   // where out jpeg is stored, must be sizes appropriately
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
  Conf.ImageQuality = quality;

  // get the numbers of mcus and appropriate function to rgb to ycbcr
  JPEG_GetEncodeColorConvertFunc(&Conf, &pRGBToYCbCr_Convert_Function, &ycbcr_mcu);

  // convert rgb to ycbcr mcu in one go
  pRGBToYCbCr_Convert_Function(img, ycbcr_tmp, 0, (img_x * img_y * img_bytpp), &ycbcr_size);

  HAL_JPEG_ConfigEncoding(hjpeg, &Conf);

  // convert ycbcr mcus to jpeg in one go
  HAL_JPEG_Encode(hjpeg, ycbcr_tmp, ycbcr_size, jpg_out, (img_x * img_y * img_bytpp), 1000);

  // JPEG_Size is updated in HAL_JPEG_DataReadyCallback
  return JPEG_Size;
}

void JPEG_Encode_HW_DMA(JPEG_HandleTypeDef *hjpeg,
                        uint8_t *img,       // pointer input rgb map
                        uint32_t img_x,     // image width
                        uint32_t img_y,     // image hight
                        uint8_t img_bytpp,  // bytes per pixel
                        uint8_t quality,    // out jpeg quality
                        uint8_t *ycbcr_tmp, // temperory buffer for ycbcr conversion, must be sizes appropriately
                        uint8_t *jpg_out)   // where out jpeg is stored, must be sizes appropriately
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
  Conf.ImageQuality = quality;

  // get the numbers of mcus and appropriate function to rgb to ycbcr
  JPEG_GetEncodeColorConvertFunc(&Conf, &pRGBToYCbCr_Convert_Function, &ycbcr_mcu);

  uint32_t tick = HAL_GetTick();
  // convert rgb to ycbcr mcu in one go
  pRGBToYCbCr_Convert_Function(img, ycbcr_tmp, 0, (img_x * img_y * img_bytpp), &ycbcr_size);

  tick = HAL_GetTick() - tick;
  HAL_JPEG_ConfigEncoding(hjpeg, &Conf);

  YCBCR_Index = 0x00;
  YCBCR_Size = ycbcr_size;
  YCBCR_IMG = ycbcr_tmp;
  YCBCR_Packet_Size = 8 * 1024;
  YCBCR_Packets = YCBCR_Size / YCBCR_Packet_Size;
  YCBCR_Index++;
  YCBCR_IMG += YCBCR_Packet_Size;

  SCB_CleanInvalidateDCache();
  SCB_InvalidateICache();

  // feed ycbcr mcus to jpeg in blocks of YCBCR_Packet_Size
  HAL_JPEG_Encode_DMA(hjpeg, ycbcr_tmp, YCBCR_Packet_Size, jpg_out, 32 * 1024);
}

// poll encoding status if using dma mode
uint8_t JPEG_Get_Status(uint32_t *size)
{
  uint8_t tmp = Encoded_Flag;
  *size = JPEG_Size;
  Encoded_Flag = 0;
  return tmp;
}
//////////////////////////////////////// callbacks and ISRS ////////////////////////////////////////////
/**
  * @brief JPEG Get Data callback
  * @param hjpeg: JPEG handle pointer
  * @param NbEncodedData: Number of encoded (consummed) bytes from input buffer
  * @retval None
  */
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbEncodedData)
{
  if (DMA_Mode)
  {
    if (YCBCR_Index == YCBCR_Packets)
    {
      YCBCR_Packet_Size = YCBCR_Size % YCBCR_Packet_Size;
    }

    // feed ycbcr mcus to jpeg in blocks of YCBCR_Packet_Size
    HAL_JPEG_ConfigInputBuffer(hjpeg, YCBCR_IMG, YCBCR_Packet_Size);
    YCBCR_Index++;
    YCBCR_IMG += YCBCR_Packet_Size;

    if (YCBCR_Index > YCBCR_Packets)
    {
      YCBCR_Index = 0x00;
      // if HAL_JPEG_Pause or HAL_JPEG_ConfigInputBuffer with zero length here then HAL_JPEG_DataReadyCallbackn will not be called
      HAL_JPEG_ConfigInputBuffer(hjpeg, YCBCR_IMG, 4);
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
