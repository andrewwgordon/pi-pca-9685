/** 
*   @file pca9685.h
*   @author Andrew Gordon
*   @date 20 Feb 2021 
*
*   @brief NXP PCA9685 User Mode Driver
*
*/
#ifndef _PCA9685_H
#define _PCA9685_H

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define OK      (1)
#define ERROR   (-1)

/**
 *  @brief i2c bus, either 0 or 1.
 *
 */
typedef enum pca_i2c_bus_enum
{
    PCA_I2C_BUS_0,                                          // equates to linux /dev/i2c-0
    PCA_I2C_BUS_1                                           // equates to linux /dev/i2c-1
} pca_i2c_bus_t;

/**
 *  @brief PCA9685 i2c address, default 0x40.
 *
 */
typedef enum pca9685_i2c_addr_enum
{   
    DEFAULT     =   0x40                                   // default address
} pca9685_i2c_addr_t;

/**
 *  @brief PCA address
 *
 */
typedef struct pca9685_addr_struct
{
    pca_i2c_bus_t       i2c_bus;                            // I2C bus
    pca9685_i2c_addr_t  i2c_addr;                           // I2C device address
} pca9685_addr_t;

/**
*   Aquire a connection to the PCA9685.
*   @param[in] pca9685_addr The I2C bus and devioe address.
*   @return The handle to the PCA or -1 for error.
*/
int16_t 
pca9685_connect(pca9685_addr_t pca9685_addr);

/**
*   Initialise the PCA
*   @param[in] pca9685_h Handle to the PCA.
*   @return Success or error.
*/
int8_t 
pca9685_initialise(int16_t pca9685_h);

/**
*   Set a single PWM channel.
*   @param[in] pca9685_h Handle to the PCA.
*   @param[in] channel Channel number.
*   @param[in] on On offset.
*   @param[in] off Off offset.
*   @return Success or error.
*/
int8_t
pca9685_setpwm(int16_t pca9865_h,uint8_t pca9685_channel, uint16_t on, uint16_t off);

/**
*   Set all PWM channels.
*   @param[in] pca9685_h Handle to the PCA.
*   @param[in] channel Channel number.
*   @param[in] on On offset.
*   @param[in] off Off offset.
*   @return Success or error.
*/
int8_t
pca9685_setallpwm(int16_t pca9865_h,uint16_t on, uint16_t off);

/**
*   Set PWM frequency in Hz.
*   @param[in] pca9685_h Handle to the PCA.
*   @param[in] freq 
*   @return Success or error.
*/
int8_t
pca9685_setfreq(int16_t pca9865_h,uint8_t freq);
#endif