/**
 * @file tft.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "lvgl/lvgl.h"
#include <string.h>
#include <stdlib.h>

#include "tft.h"
#include "ltdc.h"
#include "dma.h"
/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_DEPTH != 16 && LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32
#error LV_COLOR_DEPTH must be 16, 24, or 32
#endif

/**
  * @brief  LCD status structure definition
  */
#define LCD_OK                          ((uint8_t)0x00)
#define LCD_ERROR                       ((uint8_t)0x01)
#define LCD_TIMEOUT                     ((uint8_t)0x02)

/**********************
 *      TYPEDEFS
 **********************/
#define USE_DOUBLE_BUUFER               1

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*These 3 functions are needed by LittlevGL*/
static void ex_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t * color_p);
#if LV_USE_GPU
#endif

static uint8_t LCD_Init(void);

static void DMA_Config(void);
static void DMA_TransferComplete(DMA_HandleTypeDef *han);
static void DMA_TransferError(DMA_HandleTypeDef *han);

#if LV_USE_GPU
static void DMA2D_Config(void);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_GPU

#endif

#if LV_COLOR_DEPTH == 16
typedef uint16_t uintpixel_t;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
typedef uint32_t uintpixel_t;
#endif

static lv_disp_buf_t disp_buf_1;

#if(USE_DOUBLE_BUUFER == 1)
__attribute__((section(".SD_RAM")))  static lv_color_t buf1_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
__attribute__((section(".SD_RAM")))  static lv_color_t buf1_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];
#else
/* You can try to change buffer to internal ram by uncommenting line below and commenting
 * SDRAM one. */
__attribute__((section(".SD_RAM"))) static uintpixel_t my_fb[TFT_HOR_RES * TFT_VER_RES];
static lv_color_t buf1_1[LV_HOR_RES_MAX * 68];
static lv_color_t buf1_2[LV_HOR_RES_MAX * 68];

static int32_t            x1_flush;
static int32_t            y1_flush;
static int32_t            x2_flush;
static int32_t            y2_fill;
static int32_t            y_fill_act;
static const lv_color_t * buf_to_flush;
#endif
static lv_disp_t *our_disp = NULL;

static uintpixel_t *current_frame_buffer;
static uint8_t frame_refresh_flag;
/**********************
 *      MACROS
 **********************/

/**
 * Initialize your display here
 */

void tft_init(void)
{
	/* There is only one display on STM32 */
	if(our_disp != NULL)
		abort();
    /* LCD Initialization */
    LCD_Init();

    DMA_Config();

#if LV_USE_GPU != 0
    DMA2D_Config();
#endif
   /*-----------------------------
	* Create a buffer for drawing
	*----------------------------*/
#if(USE_DOUBLE_BUUFER == 1)
	lv_disp_buf_init(&disp_buf_1, buf1_1, buf1_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/
#else
	lv_disp_buf_init(&disp_buf_1, buf1_1, buf1_2, LV_HOR_RES_MAX * 68);   /*Initialize the display buffer*/
#endif
	/*-----------------------------------
	* Register the display in LittlevGL
	*----------------------------------*/

	lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
	lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

	/*Set up the functions to access to your display*/

	/*Set the resolution of the display*/
	disp_drv.hor_res = LV_HOR_RES_MAX;
	disp_drv.ver_res = LV_VER_RES_MAX;

	/*Used to copy the buffer's content to the display*/
	disp_drv.flush_cb = ex_disp_flush;

	/*Set a display buffer*/
	disp_drv.buffer = &disp_buf_1;


	/*Finally register the driver*/
	our_disp = lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_flush_ready()' has to be called when finished
 * This function is required only when LV_VDB_SIZE != 0 in lv_conf.h*/
static void ex_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t * color_p)
{
#if 0
	int32_t x1 = area->x1;
	int32_t x2 = area->x2;
	int32_t y1 = area->y1;
	int32_t y2 = area->y2;
    /*Return if the area is out the screen*/

    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > TFT_HOR_RES - 1) return;
    if(y1 > TFT_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > TFT_HOR_RES - 1 ? TFT_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > TFT_VER_RES - 1 ? TFT_VER_RES - 1 : y2;

    x1_flush = act_x1;
    y1_flush = act_y1;
    x2_flush = act_x2;
    y2_fill = act_y2;
    y_fill_act = act_y1;
    buf_to_flush = color_p;

	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
    /*##-7- Start the DMA transfer using the interrupt mode #*/
    /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
    /* Enable All the DMA interrupts */
    HAL_StatusTypeDef err;
    uint32_t length = (x2_flush - x1_flush + 1);
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
    err = HAL_DMA_Start_IT(&hdma_memtomem_dma1_stream0,(uint32_t)buf_to_flush, (uint32_t)&my_fb[y_fill_act * TFT_HOR_RES + x1_flush], length);
    if(err != HAL_OK)
    {
        while(1);	/*Halt on error*/
    }
#else
	current_frame_buffer = color_p;
	frame_refresh_flag = 1;
	HAL_LTDC_SetAddress(&hltdc, (uint32_t)color_p, 0);
	lv_disp_flush_ready(&our_disp->driver);
#endif
}

/**
  * @brief  Initializes the LCD.
  * @retval LCD state
  */
static uint8_t LCD_Init(void)
{
	// LTDC congigured in cube
#if(USE_DOUBLE_BUUFER == 0)
	current_frame_buffer = my_fb;
	HAL_LTDC_SetAddress(&hltdc, (uint32_t)my_fb, 0);
#endif
    return LCD_OK;
}

static void DMA_Config(void)
{
	// DMA congigured in cube

    /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
    HAL_DMA_RegisterCallback(&hdma_memtomem_dma1_stream0, HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferComplete);
    HAL_DMA_RegisterCallback(&hdma_memtomem_dma1_stream0, HAL_DMA_XFER_ERROR_CB_ID, DMA_TransferError);
}

/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void DMA_TransferComplete(DMA_HandleTypeDef *han)
{
#if(USE_DOUBLE_BUUFER == 0)
    y_fill_act ++;

    if(y_fill_act > y2_fill) {
    	SCB_CleanInvalidateDCache();
    	SCB_InvalidateICache();
        lv_disp_flush_ready(&our_disp->driver);
    } else {
    	uint32_t length = (x2_flush - x1_flush + 1);
        buf_to_flush += x2_flush - x1_flush + 1;
        /*##-7- Start the DMA transfer using the interrupt mode ####################*/
        /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
        /* Enable All the DMA interrupts */
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
        length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
        if(HAL_DMA_Start_IT(han,(uint32_t)buf_to_flush, (uint32_t)&my_fb[y_fill_act * TFT_HOR_RES + x1_flush], length) != HAL_OK)
        {
            while(1);	/*Halt on error*/
        }
    }
#endif
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void DMA_TransferError(DMA_HandleTypeDef *han)
{

}

#if LV_USE_GPU != 0

/**
  * @brief  DMA2D Transfer completed callback
  * @param  hdma2d: DMA2D handle.
  * @note   This example shows a simple way to report end of DMA2D transfer, and
  *         you can add your own implementation.
  * @retval None
  */
static void DMA2D_TransferComplete(DMA2D_HandleTypeDef *hdma2d)
{

}

/**
  * @brief  DMA2D error callbacks
  * @param  hdma2d: DMA2D handle
  * @note   This example shows a simple way to report DMA2D transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
static void DMA2D_TransferError(DMA2D_HandleTypeDef *hdma2d)
{

}

/**
  * @brief DMA2D configuration.
  * @note  This function Configure the DMA2D peripheral :
  *        1) Configure the Transfer mode as memory to memory with blending.
  *        2) Configure the output color mode as RGB565 pixel format.
  *        3) Configure the foreground
  *          - first image loaded from FLASH memory
  *          - constant alpha value (decreased to see the background)
  *          - color mode as RGB565 pixel format
  *        4) Configure the background
  *          - second image loaded from FLASH memory
  *          - color mode as RGB565 pixel format
  * @retval None
  */
static void DMA2D_Config(void)
{
	//TODO
}
#endif

uint8_t LCD_Get_Frame_Buffer(uint8_t **buffer)
{
	uint8_t tmp = frame_refresh_flag;
	*buffer = (uint8_t*)current_frame_buffer;
	frame_refresh_flag = 0;
	return tmp;
}
