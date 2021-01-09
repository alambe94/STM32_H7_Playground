/**
 * @file indev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "hal_stm_lvgl/tft/tft.h"
#include "lvgl/src/lv_hal/lv_hal.h"
#include "touch_driver.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

/**********************
 *  STATIC VARIABLES
 **********************/
//static TS_State_t  TS_State;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize your input devices here
 */
void touchpad_init(void)
{
    Touch_Init();

    lv_indev_drv_t indev_drv;                       /*Descriptor of an input device driver*/
    lv_indev_drv_init(&indev_drv);                  /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;         /*The touchpad is pointer type device*/
    indev_drv.read_cb = touchpad_read;

    lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Read an input device
 * @param indev_id id of the input device to read
 * @param x put the x coordinate here
 * @param y put the y coordinate here
 * @return true: the device is pressed, false: released
 */
static bool touchpad_read(lv_indev_drv_t *indev, lv_indev_data_t *data)
{
    /* Read your touchpad */
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    FT5336_State_t ts;

    Touch_Get_State(&ts);

    if(ts.TouchDetected) {
            data->point.x = ts.TouchX;
            data->point.y = ts.TouchY;
            last_x = data->point.x;
            last_y = data->point.y;
            data->state = LV_INDEV_STATE_PR;
    } else {
            data->point.x = last_x;
            data->point.y = last_y;
            data->state = LV_INDEV_STATE_REL;
    }

    return false;   /*false: no more data to read because we are no buffering*/
}
