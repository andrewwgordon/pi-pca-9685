#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "pca9685.h"

#define SERVO_MIN   (150)
#define SERVO_MAX   (600)

int
main(void)
{
    pca9685_addr_t      pca9685_addr;
    int16_t             pca9685_h;

    // Set the I2C bus and device address
    //
    pca9685_addr.i2c_bus = PCA_I2C_BUS_1;
    pca9685_addr.i2c_addr = DEFAULT;
    // Connect to the PCA
    //
    pca9685_h = pca9685_connect(pca9685_addr);
    if (pca9685_h != ERROR)
    {
        printf("Connected to PCA9685...\n");
    }
    else
    {
        perror("Error");
        return errno;
    }
    // Initialises the PCA
    //
    if (pca9685_initialise(pca9685_h) != ERROR)
    {
        printf("PCA initialised.\n");
    }
    else
    {
        perror("Error");
        return errno;
    }
    if (pca9685_setfreq(pca9685_h,60) == ERROR)
    {
        perror("Error");
        return errno;
    }
    // Continously cycle a servo on channel 0 between MIN and MAX pulse widths.
    //
    while (true)
    {
        if (pca9685_setpwm(pca9685_h,0,0,SERVO_MIN) == ERROR)
        {
            perror("Error");
        }
        sleep(1);
        if (pca9685_setpwm(pca9685_h,0,0,SERVO_MAX) == ERROR)
        {
            perror("Error");
        }
        sleep(1);
    }
    return 0;
}