/*
 * tfluna_i2c.c
 *
 *  Created on: Sep 16, 2021
 *      Author: AORUS
 */


/* File Name: tfluna_i2c.c
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
 *
 * Default settings for the TF-Luna are:
 *    0x10  -  default slave device I2C address `TFL_DEF_ADR`
 *    100Hz  - default data frame-rate `TFL_DEF_FPS`
 *
 *  There are is two important function: 'getData'and 'TF_Luna_init'.
 *  `getData( tf_luna, dist, flux, temp)`
 *  `TF_Luna_init(tf_luna, i2c, addr)`
 *
      Reads the disance measured, return signal strength and chip temperature.
      - dist : unsigned integer : distance measured by the device, in cm.
      - flux : unsigned integer : signal strength, quality or confidence
               If flux value too low, an error will occur.
      - temp : unsigned integer : temperature of the chip in 0.01 degrees C
      - addr : unsigned byte : address of slave device.
      Returns true, if no error occurred.
         If false, error is defined by a status code
         that can be displayed using 'printFrame()' function.

 *
 *  There are several explicit commands
 */

#include <tfluna_i2c.h>        //  TFLI2C library header
#include <stdint.h>
#include <main.h>
// Constructor/Destructor

extern I2C_HandleTypeDef hi2c1;

// Init your device or devices.
// Supported multiple devices on one i2c line.

bool TF_Luna_init(TF_Luna_Lidar *tf_luna,I2C_HandleTypeDef *i2c,uint8_t TF_Luna_address)
{

	  tf_luna->i2c = i2c;
	  tf_luna->TF_Luna_address=TF_Luna_address;
	  return 1;

}



// - - - - - - - - - - - - - - - - - - - - - - - - - -
//             GET DATA FROM THE DEVICE
// - - - - - - - - - - - - - - - - - - - - - - - - - -


bool getData(TF_Luna_Lidar *tf_luna, int16_t *dist, int16_t *flux, int16_t *temp)
{
    tfStatus = TFL_READY;    // clear status of any error condition

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 1 - Use the `HAL_I2C_MASTER_Receive` function `readReg` to fill the six byte
    // `dataArray` from the contiguous sequence of registers `TFL_DIST_LO`
    // to `TFL_TEMP_HI` that declared in the header file 'tfluna_i2c.h`.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    for (uint8_t reg = TFL_DIST_LO; reg <= TFL_TEMP_HI; reg++)
    {
      if( !readReg(tf_luna, reg)) return false;
          else dataArray[ reg] = regReply;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 2 - Shift data from read array into the three variables
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   *dist = dataArray[ 0] + ( dataArray[ 1] << 8);
   *flux = dataArray[ 2] + ( dataArray[ 3] << 8);
   *temp = dataArray[ 4] + ( dataArray[ 5] << 8);



    // Convert temperature from hundredths
    // of a degree to a whole number
   *temp = *temp / 100;
  //  *temp = *temp * 9 / 5 + 32;
    // Then convert Celsius to degrees Fahrenheit


    // - - Evaluate Abnormal Data Values - -
    // Signal strength <= 100
    if( *flux < (int16_t)100)
    {
      tfStatus = TFL_WEAK;
      return false;
    }
    // Signal Strength saturation
    else if( *flux == (int16_t)0xFFFF)
    {
      tfStatus = TFL_STRONG;
      return false;
    }
    else
    {
      tfStatus = TFL_READY;
      return true;
    }

}



// - - - - - - - - - - - - - - - - - - - - - - - - - -
//              EXPLICIT COMMANDS
// - - - - - - - - - - - - - - - - - - - - - - - - - -

//  = =  GET DEVICE TIME (in milliseconds) = = =
//  Pass back time as an unsigned 16-bit variable
bool Get_Time(TF_Luna_Lidar *tf_luna, uint16_t *tim)
{
    // Recast the address of the unsigned integer `tim`
    // as a pointer to an unsigned byte `p_tim`...
    uint8_t * p_tim = (uint8_t *) *&tim;

    // ... then address the pointer as an array.
    if( !readReg( tf_luna, TFL_TICK_LO)) return false;
        else p_tim[ 0] = regReply;  // Read into `tim` array
    if( !readReg( tf_luna, TFL_TICK_HI)) return false;
        else p_tim[ 1] = regReply;  // Read into `tim` array
    return true;
}

//  = =  GET PRODUCTION CODE (Serial Number) = = =
// When you pass an array as a parameter to a function
// it decays into a pointer to the first element of the array.
// The 14 byte array variable `tfCode` declared in the example
// sketch decays to the array pointer `p_cod`.
bool Get_Prod_Code(TF_Luna_Lidar *tf_luna, uint8_t * p_cod)
{
   for (uint8_t i = 0; i < 14; ++i)
    {
      if( !readReg(tf_luna, ( 0x10 + i))) return false;
        else p_cod[ i] = regReply;  // Read into product code array
    }
    return true;
}

//  = = = =    GET FIRMWARE VERSION   = = = =
// The 3 byte array variable `tfVer` declared in the
// example sketch decays to the array pointer `p_ver`.
bool Get_Firmware_Version(TF_Luna_Lidar *tf_luna, uint8_t * p_ver)
{
    for (uint8_t i = 0; i < 3; ++i)
    {
      if( !readReg( tf_luna, ( 0x0A + i))) return false;
        else p_ver[ i] = regReply;  // Read into version array
    }
    return true;
}

//  = = = = =    SAVE SETTINGS   = = = = =
bool Save_Settings( TF_Luna_Lidar *tf_luna)
{
    return( writeReg( tf_luna,TFL_SAVE_SETTINGS, 1));
}

//  = = = =   SOFT (SYSTEM) RESET   = = = =
bool Soft_Reset( TF_Luna_Lidar *tf_luna)
{
    return( writeReg(tf_luna, TFL_SOFT_RESET, 2));
}

//  = = = = = =    SET I2C ADDRESS   = = = = = =
// Range: 0x08, 0x77. Must reboot to take effect.
bool Set_I2C_Addr(TF_Luna_Lidar *tf_luna, uint8_t adrNew)
{
    return( writeReg( tf_luna,TFL_SET_I2C_ADDR, adrNew));
}

//  = = = = =   SET ENABLE   = = = = =
bool Set_Enable( TF_Luna_Lidar *tf_luna)
{
    return( writeReg(tf_luna, TFL_DISABLE, 1));
}

//  = = = = =   SET DISABLE   = = = = =
bool Set_Disable( TF_Luna_Lidar *tf_luna)
{
    return( writeReg( tf_luna, TFL_DISABLE, 0));
}

//  = = = = = =    SET FRAME RATE   = = = = = =
bool Set_Frame_Rate(TF_Luna_Lidar *tf_luna, uint16_t *frm)
{
    // Recast the address of the unsigned integer `frm`
    // as a pointer to an unsigned byte `p_frm` ...
    uint8_t * p_frm = (uint8_t *) *&frm;

    // ... then address the pointer as an array.
    if( !writeReg( tf_luna,( TFL_FPS_LO),  p_frm[ 0])) return false;
    if( !writeReg( tf_luna,( TFL_FPS_HI),  p_frm[ 1])) return false;
    return true;
}

//  = = = = = =    GET FRAME RATE   = = = = = =
bool Get_Frame_Rate( TF_Luna_Lidar *tf_luna, uint16_t *frm)
{
    uint8_t * p_frm = (uint8_t *) *&frm;
    if( !readReg( tf_luna, TFL_FPS_LO)) return false;
        else p_frm[ 0] = regReply;  // Read into `frm` array
    if( !readReg( tf_luna, TFL_FPS_HI)) return false;
        else p_frm[ 1] = regReply;  // Read into `frm` array
    return true;
}

//  = = = =   HARD RESET to Factory Defaults  = = = =
bool Hard_Reset( TF_Luna_Lidar *tf_luna)
{
    return( writeReg( tf_luna, TFL_HARD_RESET, 1));
}

//  = = = = = =   SET CONTINUOUS MODE   = = = = = =
// Sample LiDAR chip continuously at Frame Rate
bool Set_Cont_Mode( TF_Luna_Lidar *tf_luna)
{
    return( writeReg(tf_luna, TFL_SET_TRIG_MODE,  0));
}

//  = = = = = =   SET TRIGGER MODE   = = = = = =
// Device will sample only once when triggered
bool Set_Trig_Mode( TF_Luna_Lidar *tf_luna)
{
    return( writeReg(tf_luna, TFL_SET_TRIG_MODE, 1));
}

//  = = = = = =   SET TRIGGER   = = = = = =
// Trigger device to sample once
bool Set_Trigger( TF_Luna_Lidar *tf_luna)
{
    return( writeReg(tf_luna, TFL_TRIGGER, 1));
}
//
// = = = = = = = = = = = = = = = = = = = = = = = =

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//       READ OR WRITE A GIVEN REGISTER OF THE SLAVE DEVICE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool readReg( TF_Luna_Lidar *tf_luna, uint8_t nmbr)
{


  if( HAL_I2C_Master_Transmit(tf_luna->i2c, tf_luna->TF_Luna_address<<1, &nmbr, 1, 1000) != 0)  // If write error...
  {
    tfStatus = TFL_I2CWRITE;        // then set status code...
    return false;                   // and return `false`.
  }
  // Request 1 byte from the device
  // and release bus when finished.


   if( HAL_I2C_Master_Receive(tf_luna->i2c, tf_luna->TF_Luna_address<<1, &regReply, 1, 1000) != 0)            // If read error...
    {
      tfStatus = TFL_I2CREAD;         // then set status code.
      return false;
    }

  return true;
}

bool writeReg( TF_Luna_Lidar *tf_luna, uint8_t nmbr, uint8_t data)
{


  if( HAL_I2C_Mem_Write(tf_luna->i2c, tf_luna->TF_Luna_address<<1, nmbr, 1, &data, 1, 1000) != 0)  // If write error...
  {
    tfStatus = TFL_I2CWRITE;        // then set status code...
    return false;                   // and return `false`.
  }
  else return true;

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - -    The following is for testing purposes    - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Called by either `printFrame()` or `printReply()`
// Print status condition either `READY` or error type
void printStatus()
{


	printf("Status: ");
	if( tfStatus == TFL_READY)          printf( "READY");
	    else if( tfStatus == TFL_SERIAL)    printf( "SERIAL");
	    else if( tfStatus == TFL_HEADER)    printf( "HEADER");
	    else if( tfStatus == TFL_CHECKSUM)  printf( "CHECKSUM");
	    else if( tfStatus == TFL_TIMEOUT)   printf( "TIMEOUT");
	    else if( tfStatus == TFL_PASS)      printf( "PASS");
	    else if( tfStatus == TFL_FAIL)      printf( "FAIL");
	    else if( tfStatus == TFL_I2CREAD)   printf( "I2C-READ");
	    else if( tfStatus == TFL_I2CWRITE)  printf( "I2C-WRITE");
	    else if( tfStatus == TFL_I2CLENGTH) printf( "I2C-LENGTH");
	    else if( tfStatus == TFL_WEAK)      printf( "Signal weak");
	    else if( tfStatus == TFL_STRONG)    printf( "Signal strong");
	    else if( tfStatus == TFL_FLOOD)     printf( "Ambient light");
	    else if( tfStatus == TFL_INVALID)   printf( "No Command");
	    else printf( "OTHER");


    printf("\n");

}


// Print error type and HEX values
// of each byte in the data frame
void printDataArray()
{
    printStatus();
    // Print the Hex value of each byte of data
  //  Serial.print(" Data:");
    printf("Data: ");
    for( uint8_t i = 0; i < 6; i++)
    {
    	printf("");
    	printf(dataArray[ i] < 16 ? "0" : "");
    	printf("%X ", dataArray[ i]);
  //    Serial.print(" ");
  //    Serial.print( dataArray[ i] < 16 ? "0" : "");
 //     Serial.print( dataArray[ i], HEX);
    }
    printf("\n");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

