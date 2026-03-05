# Raspberry Pi — NXP PCA9685 User-Mode Driver

A small user-space C driver for the NXP PCA9685 16-channel 12-bit PWM I²C LED controller, intended for use on Raspberry Pi (or other Linux systems exposing /dev/i2c-*). This repository provides a minimal API to connect to the device, initialize it, set PWM frequency, and drive individual or all channels.

Table of contents
- Project overview
- Requirements
- Building
- Example program
- API reference
  - Types and constants
  - Functions (signatures, parameters, return values)
- Usage examples
  - Simple servo example
  - Set all channels
- Notes and troubleshooting
- License

Project overview
----------------
This project exposes a tiny C API (pca9685.h / pca9685.c) that wraps low-level I²C operations (via an i2cdev helper) to control a PCA9685 device. It is designed to be easy to embed into small applications that run on a Raspberry Pi and similar systems.

Requirements
------------
- Linux with I²C support (kernel i2c-dev, /dev/i2c-1 typically on Raspberry Pi)
- A working i2cdev helper library/header (this code includes "i2cdev.h" and links against its implementation; provide or implement i2cdev.h / i2cdev.c as appropriate)
- gcc (or another C compiler)

Building
--------
Compile the example program (adjust include/library flags if your i2cdev implementation requires them):

```bash
gcc pca9685_example.c pca9685.c i2cdev.c -o pca9685-example
```

Or, to compile only the library and example separately:

```bash
gcc -c pca9685.c -o pca9685.o
gcc pca9685_example.c pca9685.o i2cdev.o -o pca9685-example
```

Example program
---------------
The included example (pca9685_example.c) demonstrates connecting to the device, initializing it, setting the PWM frequency, and continuously cycling a servo on channel 0.

Run (requires access to /dev/i2c-1 and appropriate permissions):

```bash
sudo ./pca9685-example
```

API reference
--------------
Header: pca9685.h

Types and constants
- OK (1) — operation succeeded.
- ERROR (-1) — operation failed.

Enums and structs
- pca_i2c_bus_t
  - PCA_I2C_BUS_0 — corresponds to /dev/i2c-0
  - PCA_I2C_BUS_1 — corresponds to /dev/i2c-1 (Raspberry Pi default)

- pca9685_i2c_addr_t
  - DEFAULT — 0x40 (default PCA9685 I²C address)

- pca9685_addr_t
  - Fields:
    - i2c_bus (pca_i2c_bus_t) — which /dev/i2c-N bus to use
    - i2c_addr (pca9685_i2c_addr_t) — device address (DEFAULT provided)

Functions
- int16_t pca9685_connect(pca9685_addr_t pca9685_addr)
  - Description: Initialize and open an I²C connection to the PCA9685 device using the supplied bus and address.
  - Parameters: pca9685_addr — bus and I²C address.
  - Returns: A non-negative handle (int16_t) on success, or ERROR (-1) on failure.

- int8_t pca9685_initialise(int16_t pca9685_h)
  - Description: Initialize the PCA9685 registers for normal operation (sets outputs, clears sleep, sets mode registers).
  - Parameters: pca9685_h — handle returned by pca9685_connect.
  - Returns: OK (1) on success, ERROR (-1) on failure.

- int8_t pca9685_setpwm(int16_t pca9685_h, uint8_t channel, uint16_t on, uint16_t off)
  - Description: Set PWM for a single channel.
  - Parameters:
    - pca9685_h — device handle
    - channel — channel index (0..15)
    - on — 12-bit on offset (0..4095)
    - off — 12-bit off offset (0..4095)
  - Returns: OK (1) on success, ERROR (-1) on failure.

- int8_t pca9685_setallpwm(int16_t pca9685_h, uint16_t on, uint16_t off)
  - Description: Set the same PWM on/off values for all 16 channels.
  - Parameters: pca9685_h, on, off (same ranges as pca9685_setpwm)
  - Returns: OK (1) on success, ERROR (-1) on failure.

- int8_t pca9685_setfreq(int16_t pca9685_h, uint16_t freq)
  - Description: Configure PWM frequency (in Hz). Internally computes the PRESCALE register from a 25 MHz reference clock and the desired frequency.
  - Parameters: pca9685_h — device handle; freq — desired frequency in Hz (typical servo use: 50–60 Hz; LED PWM commonly 1000+ Hz but limited by hardware range).
  - Returns: OK (1) on success, ERROR (-1) on failure.

Usage examples
--------------
1) Simple servo (based on pca9685_example.c)

```c
#include "pca9685.h"
#include <stdio.h>

int main(void) {
    pca9685_addr_t addr = { .i2c_bus = PCA_I2C_BUS_1, .i2c_addr = DEFAULT };
    int16_t h = pca9685_connect(addr);
    if (h == ERROR) { perror("connect"); return 1; }

    if (pca9685_initialise(h) == ERROR) { perror("init"); return 1; }
    if (pca9685_setfreq(h, 60) == ERROR) { perror("setfreq"); return 1; }

    // set channel 0 to a given pulse width (typical servo range in library: 100..600)
    pca9685_setpwm(h, 0, 0, 150); // short pulse
    sleep(1);
    pca9685_setpwm(h, 0, 0, 450); // long pulse
    sleep(1);
    return 0;
}
```

Notes: The example uses 12-bit offsets (0..4095) for the on/off timings; for servos the library's example maps typical pulse widths into this range (e.g., SERVO_MIN=100, SERVO_MAX=600).

2) Fade or set all channels

```c
// set all channels to full off
pca9685_setallpwm(h, 0, 0);

// set all channels to a mid-point duty cycle (example)
uint16_t mid = 2048;
pca9685_setallpwm(h, 0, mid);
```

Error handling and tips
----------------------
- All functions return OK (1) or ERROR (-1) except pca9685_connect which returns a handle >= 0 or ERROR.
- Check that the I²C kernel module is loaded and that /dev/i2c-1 exists. On Raspberry Pi, enable I²C via raspi-config if needed.
- Ensure the user has permission to access /dev/i2c-1 or run the binary with sudo.
- The code depends on an i2cdev implementation (i2cdev.h / i2cdev.c). If not present, provide your own thin wrapper around ioctl/I2C_RDWR or use a known i2cdev helper.

Hardware notes
--------------
- PCA9685 uses a 25 MHz internal oscillator reference; frequency computations in pca9685_setfreq() assume this reference.
- Typical servo frequency is 50–60 Hz; LEDs commonly use higher frequencies for flicker-free operation.
- Ensure proper power supply and common ground between the Pi and the PCA9685 board.

Troubleshooting
---------------
- I2C device not found: verify wiring (SDA/SCL), run `i2cdetect -y 1` (you may need i2c-tools installed) to confirm the device address (default 0x40).
- Permission denied when opening /dev/i2c-1: run with sudo or add your user to the i2c group.
- Strange PWM behavior: check wiring, servo specs, and that the PCA9685 is properly initialised (call pca9685_initialise right after connecting).

License
-------
See LICENSE file in this repository.

References
----------
- PCA9685 datasheet: https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf

Contact
-------
For questions or contributions, open an issue or submit a PR on the repository.
