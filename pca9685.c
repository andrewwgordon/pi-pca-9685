/** 
*   @file pca9685.c
*   @author Andrew Gordon
*   @date 20 Feb 2021 
*
*   @brief NXP PCA9685 User Mode Driver.
*
*/
#include <math.h>

#include "i2cdev.h"
#include "pca9685.h"

#define RESTART         0x80
#define SLEEP           0x10
#define ALLCALL         0x01
#define INVRT           0x10
#define OUTDRV          0x04

/**
 *  @brief PCA9685 regsiters as per https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf
 * 
 */
typedef enum reg_enum
{
    MODE1          = 0x00,
    MODE2          = 0x01,
    SUBADR1        = 0x02,
    SUBADR2        = 0x03,
    SUBADR3        = 0x04,
    PRESCALE       = 0xFE,
    LED0_ON_L      = 0x06,
    LED0_ON_H      = 0x07,
    LED0_OFF_L     = 0x08,
    LED0_OFF_H     = 0x09,
    ALL_LED_ON_L   = 0xFA,
    ALL_LED_ON_H   = 0xFB,
    ALL_LED_OFF_L  = 0xFC,
    ALL_LED_OFF_H  = 0xFD
} reg_t;

/**
*   Aquire a connection to the PCA9685.
*   @param[in] pca9685_addr The I2C bus and devioe address.
*   @return The handle to the PCA or -1 for error.
*/
int16_t 
pca9685_connect(pca9685_addr_t addr)
{
    int16_t pca9685_h;

    // Initialise the I2C bus at the device address.
    //
    pca9685_h = i2cdev_init(addr.i2c_bus,addr.i2c_addr);
    if (pca9685_h != ERROR)
    {
        return pca9685_h;
    }
    return ERROR;
}

/**
*   Initialise the PCA
*   @param[in] pca9685_h Handle to the PCA.
*   @return Sucess or error.
*/
int8_t 
pca9685_initialise(int16_t pca9685_h)
{
    uint8_t mode1;

    if (pca9685_setallpwm(pca9685_h,0,0) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,MODE2,OUTDRV) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,MODE1,ALLCALL) == ERROR)
    {
        return ERROR;
    }
    usleep(5000);
    if (i2cdev_readbyte(pca9685_h,MODE1,&mode1) == ERROR)
    {
        return ERROR;
    }
    mode1 = mode1 & ~SLEEP;
    if (i2cdev_writebyte(pca9685_h,MODE1,mode1) == ERROR)
    {
        return ERROR;
    }
    usleep(5000);
    return OK;
}

/**
*   Set a single PWM channel.
*   @param[in] pca9685_h Handle to the PCA.
*   @param[in] channel Channel number.
*   @param[in] on On offset.
*   @param[in] off Off offset.
*   @return Success or error.
*/
int8_t
pca9685_setpwm(int16_t pca9685_h,uint8_t channel, uint16_t on, uint16_t off)
{
    if (i2cdev_writebyte(pca9685_h,LED0_ON_L+4*channel,on & 0xFF) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,LED0_ON_H+4*channel,on >> 8) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,LED0_OFF_L+4*channel,off & 0xFF) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,LED0_OFF_H+4*channel,off >> 8) == ERROR)
    {
        return ERROR;
    }
    return OK;
}

/**
*   Set all PWM channels.
*   @param[in] pca9685_h Handle to the PCA.
*   @param[in] channel Channel number.
*   @param[in] on On offset.
*   @param[in] off Off offset.
*   @return Success or error.
*/
int8_t
pca9685_setallpwm(int16_t pca9685_h,uint16_t on, uint16_t off)
{
    if (i2cdev_writebyte(pca9685_h,ALL_LED_ON_L,on & 0xFF) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,ALL_LED_ON_H,on >> 8) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,ALL_LED_OFF_L,off & 0xFF) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,ALL_LED_OFF_H,off >> 8) == ERROR)
    {
        return ERROR;
    }
    return OK;
}

/**
*   Set PWM frequency in Hz.
*   @param[in] pca9685_h Handle to the PCA.
*   @param[in] freq 
*   @return Success or error.
*/
int8_t
pca9685_setfreq(int16_t pca9685_h,uint8_t freq)
{
    uint8_t newmode;
    uint8_t oldmode;
    double prescaleval;
    uint8_t prescale;
    
    prescaleval =   25000000.0;          // 25Mhz
    prescaleval /=  4096.0;              // 12-bit
    prescaleval /=  (double)freq;
    prescaleval -=  1.0;

    prescale = (uint8_t)floor(prescaleval + 0.5);
    if (i2cdev_readbyte(pca9685_h,MODE1,&oldmode) == ERROR)
    {
        return ERROR;
    }
    newmode = (oldmode & 0x7F) | 0x10;
    if (i2cdev_writebyte(pca9685_h,MODE1,newmode) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,PRESCALE,prescale) == ERROR)
    {
        return ERROR;
    }
    if (i2cdev_writebyte(pca9685_h,MODE1,oldmode) == ERROR)
    {
        return ERROR;
    }
    usleep(5000);
    if (i2cdev_writebyte(pca9685_h,MODE1,oldmode | 0x80) == ERROR)
    {
        return ERROR;
    }
    return OK;
}