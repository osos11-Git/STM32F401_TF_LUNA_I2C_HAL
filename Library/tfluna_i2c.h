/*
 * tfluna_i2c.h
 *
 *  Created on: Sep 16, 2021
 *      Author: AORUS
 */

#ifndef INC_TFLUNA_I2C_H_
#define INC_TFLUNA_I2C_H_



/* File Name: tfluna_i2c.h
 * Developer: Bud Ryerson
 * Ported by: osos11
 * Date:      10 JUL 2021
 * Port Date: 6 OCT 2021
 * Version:   0.1.1 - Fixed some typos in comments.
              Changed TFL_DEFAULT_ADDR and TFL_DEFAULT_FPS
              to TFL_DEF_ADDR and TFL_DEF_FPS in header file.
              Changed `printStatus` from private to public
              0.2.0 - Corrected (reversed) Enable/Disable commands
 * Described: STM32 HAL based Library for the Benewake TF-Luna Lidar sensor
 *            configured for the I2C interface
 *
 */

#include <stdint.h>    // Always include this. It's important.
#include <stdbool.h>
#include <main.h>
#define bool _Bool


typedef struct My_Lidar
{
	I2C_HandleTypeDef *i2c;
	uint8_t 	 TF_Luna_address;

}TF_Luna_Lidar;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Definitions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define TFL_DEF_ADR          0x10  // default I2C address = 16
#define TFL_DEF_FPS          0x64   // default frame-rate = 100fps

// - - - -   Register Names and Numbers   - - - -
#define TFL_DIST_LO          0x00  //R Unit: cm
#define TFL_DIST_HI          0x01  //R
#define TFL_FLUX_LO          0x02  //R
#define TFL_FLUX_HI          0x03  //R
#define TFL_TEMP_LO          0x04  //R Unit: 0.01 Celsius
#define TFL_TEMP_HI          0x05  //R
#define TFL_TICK_LO          0x06  //R Timestamp
#define TFL_TICK_HI          0x07  //R
#define TFL_ERR_LO           0x08  //R
#define TFL_ERR_HI           0x09  //R
#define TFL_VER_REV          0x0A  //R
#define TFL_VER_MIN          0x0B  //R
#define TFL_VER_MAJ          0x0C  //R

#define TFL_SAVE_SETTINGS    0x20  //W -- Write 0x01 to save
#define TFL_SOFT_RESET       0x21  //W -- Write 0x02 to reboot.
                       // Lidar not accessible during few seconds,
                       // then register value resets automatically
#define TFL_SET_I2C_ADDR     0x22  //W/R -- Range 0x08,0x77.
                       // Must reboot to take effect.
#define TFL_SET_TRIG_MODE    0x23  //W/R -- 0-continuous, 1-trigger
#define TFL_TRIGGER          0x24  //W  --  1-trigger once
#define TFL_DISABLE          0x25  //W/R -- 0-disable, 1-enable
#define TFL_FPS_LO           0x26  //W/R -- lo byte
#define TFL_FPS_HI           0x27  //W/R -- hi byte
#define TFL_SET_LO_PWR       0x28  //W/R -- 0-normal, 1-low power
#define TFL_HARD_RESET       0x29  //W  --  1-restore factory settings

/////// FPS (Low Power Mode) ///////
#define FPS_1                   0x01
#define FPS_2                   0x02
#define FPS_3                   0x03
#define FPS_4                   0x04
#define FPS_5                   0x05
#define FPS_6                   0x06
#define FPS_7                   0x07
#define FPS_8                   0x08
#define FPS_9                   0x09
#define FPS_10                  0x0A

////// FPS (High Power Mode) /////
#define FPS_35                  0x23
#define FPS_50                  0x32
#define FPS_100                 0x64
#define FPS_125                 0x7D
#define FPS_250                 0xFA

// Error Status Condition definitions
#define TFL_READY            0  // no error
#define TFL_SERIAL           1  // serial timeout
#define TFL_HEADER           2  // no header found
#define TFL_CHECKSUM         3  // checksum doesn't match
#define TFL_TIMEOUT          4  // I2C timeout
#define TFL_PASS             5  // reply from some system commands
#define TFL_FAIL             6  //           "
#define TFL_I2CREAD          7
#define TFL_I2CWRITE         8
#define TFL_I2CLENGTH        9
#define TFL_WEAK            10  // Signal Strength ≤ 100
#define TFL_STRONG          11  // Signal Strength saturation
#define TFL_FLOOD           12  // Ambient Light saturation
#define TFL_MEASURE         13
#define TFL_INVALID         14  // Invalid operation sent to sendCommand()


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                 OBJECT CLASS DEFINITIONS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //Init device
    bool TF_Luna_init(TF_Luna_Lidar *tf_luna,I2C_HandleTypeDef *i2c,uint8_t TF_Luna_address);

    // Get data
    bool getData( TF_Luna_Lidar *tf_luna,int16_t *dist, int16_t *flux, int16_t *temp);

    // Read From or Write To an I2C register
    bool readReg( TF_Luna_Lidar *tf_luna,uint8_t nmbr);
    bool writeReg( TF_Luna_Lidar *tf_luna,uint8_t nmbr, uint8_t data);

    // Explicit Device Commands
    bool Get_Firmware_Version(TF_Luna_Lidar *tf_luna, uint8_t ver[]);
    bool Get_Frame_Rate( TF_Luna_Lidar *tf_luna,uint16_t *frm);
    bool Get_Prod_Code(TF_Luna_Lidar *tf_luna, uint8_t cod[]);
    bool Get_Time( TF_Luna_Lidar *tf_luna,uint16_t *tim);

    bool Set_Frame_Rate(TF_Luna_Lidar *tf_luna, uint16_t *frm);
    bool Set_I2C_Addr(TF_Luna_Lidar *tf_luna, uint8_t adrNew);
    bool Set_Enable( TF_Luna_Lidar *tf_luna);
    bool Set_Disable(TF_Luna_Lidar *tf_luna);
    bool Soft_Reset(TF_Luna_Lidar *tf_luna);  // Reset and reboot
    bool Hard_Reset( TF_Luna_Lidar *tf_luna);  // Restore factory defaults
    bool Save_Settings( TF_Luna_Lidar *tf_luna);
    bool Set_Trig_Mode( TF_Luna_Lidar *tf_luna);
    bool Set_Cont_Mode(TF_Luna_Lidar *tf_luna);
    bool Set_Trigger(TF_Luna_Lidar *tf_luna);  // false = continuous

    //  For testing purposes: print reply data and status
    void printDataArray();
    void printStatus();

    uint8_t tfStatus;        // system error status: READY = 0
    uint8_t dataArray[ 6];
    uint8_t regReply;





#endif /* INC_TFLUNA_I2C_H_ */
