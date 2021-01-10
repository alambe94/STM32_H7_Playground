#ifndef __ENCODE_DMA_H
#define __ENCODE_DMA_H

#include "jpeg_utils.h"

uint32_t JPEG_Encode_SW(uint8_t *img,
                        uint32_t img_x,
                        uint32_t img_y,
                        uint8_t img_bytpp,
                        uint8_t quality,
                        uint8_t *jpg_out,
                        uint32_t jpg_out_sz);

uint32_t JPEG_Encode_HW(JPEG_HandleTypeDef *hjpeg,
                        uint8_t *img,
                        uint32_t img_x,
                        uint32_t img_y,
                        uint8_t img_bytpp,
                        uint8_t quality,
                        uint8_t *img_tmp,
                        uint8_t *jpg_out);

void JPEG_Encode_HW_DMA(JPEG_HandleTypeDef *hjpeg,
                        uint8_t *img,
                        uint32_t img_x,
                        uint32_t img_y,
                        uint8_t img_bytpp,
                        uint8_t quality,
                        uint8_t *img_tmp,
                        uint8_t *jpg_out);

uint8_t JPEG_Get_Status(uint32_t *size);
#endif /* __ENCODE_DMA_H */
