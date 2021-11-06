# STM32F401_TF_LUNA_I2C_HAL
 STM32F401_TF_LUNA_I2C_HAL

STM32F401CCU6 Blackpill

Based on Arduino... https://github.com/budryerson/TFLuna-I2C

STM32 F4 FW 1.26.2

STM32CubeIDE v1.7.0

HAL

Supported multiple devices on one i2c line.

Printf debugging and SWV, you should watch the video : https://youtu.be/sPzQ5CniWtw

```
ARDUINO SLAVE ADDRESS

Arduino (IDE only… not the mcu) uses 7 bit addressing system and the rest (including STM32) uses 8 bits. 
Whenever you are using the I2C address, use the full 8 bits for the address. 
You can find the address for your device in it’s datasheet. 
The device 7 bits address value in datasheet must be shifted to the left .
Slave address has been already shifting to the left in the library.

Welcome to the real world :D
```


How to use this Library: 
* Enable I2C normal or fast speed.    
* Include Header and source into your project.   
```
#include "tfluna_i2c.h"
TF_Luna_Lidar TF_Luna_1;
int16_t  tfAddr = TFL_DEF_ADR;    // default I2C address = 0x10 (7 bit)
uint16_t tfFrame = TFL_DEF_FPS;   // default frame rate
// device variables passed back by getData
int16_t  tfDist = 0 ;   // distance in centimeters
int16_t  tfFlux = 0 ;   // signal quality in arbitrary units
int16_t  tfTemp = 0 ;   // temperature in 0.01 degree Celsius
// other device variables
uint16_t tfTime = 0;    // devie clock in milliseconds
uint8_t  tfVer[3];      // device version number
uint8_t  tfCode[14];    // device serial number
// sub-loop counter for Time display
uint8_t tfCount = 0;
int main()
{
  TF_Luna_init(&TF_Luna_1, &hi2c1, 0x10); //0x10->7bit. 7bit Slave address has been already shifting to the left in the library. 
  while(1)
  {
    getData(&TF_Luna_1, &tfDist, &tfFlux, &tfTemp);
    printf(tfDist); // Open your SVW ITM Data Console
    HAL_Delay(4); // 250Hz
 // HAL_Delay(10); // 100Hz
  }
}
```
