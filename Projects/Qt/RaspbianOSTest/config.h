#ifndef CONFIG_H
#define CONFIG_H

// GPIO pin definitions
#define GPIOLED0                6
#define GPIOLED1                13
#define GPIOSW0                 17
#define GPIOSW1                 27
#define GPIOIN                  4
#define GPIOOUT                 5
// Reset pins
#define GPIOBLERST              18
#define GPIOADCRST              22

// ADC address
#define ADC_I2C_ADDR            0x48
// ADC register map
#define ADC_I2C_MODE_REG        0x00
#define ADC_I2C_INT_CNTRL_REG   0x01
#define ADC_I2C_DATA0_REG       0x02
#define ADC_I2C_DATA1_REG       0x04
#define ADC_I2C_ACQ_TIME_REG    0x14
// ADC modes and conversion start
#define ADC_I2C_MODE_AWAKE_CH0      0x80
#define ADC_I2C_MODE_AWAKE_CH1      0x81
#define ADC_I2C_DATA0_CONV_START    0xC0
#define ADC_I2C_DATA1_CONV_START    0xC1
// ADC incremental read
#define ADC_I2C_READ_INC            0x80
// ADC other defines
#define ADC_I2C_DISABLE_INT         0x00
#define ADC_I2C_MAX_ACQ_TIME        0x1F

// BLE SensorTag Address
#define SENSORTAG_ADDRESS_0         0x01
#define SENSORTAG_ADDRESS_1         0x47
#define SENSORTAG_ADDRESS_2         0xC1
#define SENSORTAG_ADDRESS_3         0x89
#define SENSORTAG_ADDRESS_4         0x71
#define SENSORTAG_ADDRESS_5         0x24
// BLE handles
//  Temperature
#define BLETEMPDATAHNDL             0x0024
#define BLETEMPCONFIGHNDL           0x0027
#define BLETEMPPERIODHNDL           0x0029
//  Light
#define BLELIGHTDATAHNDL            0x0044
#define BLELIGHTCONFIGHNDL          0x0047
#define BLELIGHTPERIODHNDL          0x0049
// BLE other defines
#define BLECONNINTERVALMIN          6
#define BLECONNINTERVALMAX          3200
#define BLECONNTIMEOUT              1000
#define BLECONNSLAVELATENCY         0
// Serial communication baudrate
#define BLEBAUDRATE                 115200

#define LOGFILENAME                 "/home/pi/logfile.txt"

#endif // CONFIG_H
