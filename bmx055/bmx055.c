/*
 * bmx055.c
 *
 *  Created on: 26-Jul-2017
 *      Author: saket
 */
#include <ti/display/Display.h>
#include "Board.h"
#include "user_i2c.h"
#include "bmx055.h"
/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>


typedef struct{
    uint8_t lo;
    int8_t hi;
}HighLow_t;

typedef union{
    HighLow_t u8;
    uint16_t  u16;
}element_t;

typedef struct
{
        element_t  x;
        element_t  y;
        element_t  z;
    //uint8_t tilt;
} RawSample_t;

RawSample_t RawSample;

//uint8_t OSR    = ADC_8192;         // set pressure amd temperature oversample rate
uint8_t Gscale = GFS_125DPS;       // set gyro full scale
uint8_t GODRBW = G_200Hz23Hz;      // set gyro ODR and bandwidth
uint8_t Ascale = AFS_2G;           // set accel full scale
uint8_t ACCBW  = 0x08 | ABW_16Hz;  // Choose bandwidth for accelerometer, need bit 3 = 1 to enable bandwidth choice in enum
uint8_t Mmode  = Regular;          // Choose magnetometer operation mode
uint8_t MODR   = MODR_10Hz;        // set magnetometer data rate
float aRes, gRes, mRes;            // scale resolutions per LSB for the sensors

// Parameters to hold BMX055 trim values
signed char   dig_x1;
signed char   dig_y1;
signed char   dig_x2;
signed char   dig_y2;
uint16_t      dig_z1;
int16_t       dig_z2;
int16_t       dig_z3;
int16_t       dig_z4;
unsigned char dig_xy1;
signed char   dig_xy2;
uint16_t      dig_xyz1;


void initBMX055()
{
    char wr_buff[10],rx_buff[10];

   // start with all sensors in default mode with all registers reset
   wr_buff[0]= BMX055_ACC_BGW_SOFTRESET;
   wr_buff[1]= 0xB6;
   i2c_operation(BMX055_ACC_ADDRESS,wr_buff,2,rx_buff,0);  // reset accelerometer
   sleep(1);// Wait for all registers to reset

   // Configure accelerometer
   wr_buff[0]= BMX055_ACC_PMU_RANGE;
   wr_buff[1]= Ascale & 0x0F;
   i2c_operation(BMX055_ACC_ADDRESS,wr_buff,2,rx_buff,0);

   wr_buff[0]= BMX055_ACC_PMU_BW;
   wr_buff[1]= ACCBW & 0x0F;
   i2c_operation(BMX055_ACC_ADDRESS,wr_buff,2,rx_buff,0);

   wr_buff[0]= BMX055_ACC_D_HBW;
   wr_buff[1]= ACCBW & 0x00;
   i2c_operation(BMX055_ACC_ADDRESS,wr_buff,2,rx_buff,0);

   wr_buff[0]= BMX055_GYRO_RANGE;
   wr_buff[1]= Gscale;
   i2c_operation(BMX055_GYRO_ADDRESS,wr_buff,2,rx_buff,0);  // set GYRO FS range

   wr_buff[0]= BMX055_GYRO_BW;
   wr_buff[1]= GODRBW;
   i2c_operation(BMX055_GYRO_ADDRESS,wr_buff,2,rx_buff,0);   // set GYRO ODR and Bandwidth

   // Configure magnetometer

   wr_buff[0]= BMX055_MAG_PWR_CNTL1;
   wr_buff[1]= 0x82;
   i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0); // Softreset magnetometer, ends up in sleep mode
   usleep(5000);

   wr_buff[0]= BMX055_MAG_PWR_CNTL1;
   wr_buff[1]= 0x01;
   i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0); // Wake up magnetometer
   usleep(5000);

   wr_buff[0]= BMX055_MAG_PWR_CNTL2;
   wr_buff[1]= MODR << 3;
   i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0); // Normal mode


// Set up four standard configurations for the magnetometer
  switch (Mmode)
  {
    case lowPower:
         // Low-power
           wr_buff[0]= BMX055_MAG_REP_XY;
           wr_buff[1]=  0x01;
           i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0);// 3 repetitions (oversampling)

           wr_buff[0]= BMX055_MAG_REP_Z;
           wr_buff[1]=  0x02;
           i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0);// 3 repetitions (oversampling)
          break;
    case Regular:
          // Regular
           wr_buff[0]= BMX055_MAG_REP_XY;
           wr_buff[1]=   0x04;
           i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0); //  9 repetitions (oversampling)

          wr_buff[0]= BMX055_MAG_REP_Z;
          wr_buff[1]=  0x16;
          i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0);// 15 repetitions (oversampling)
          break;
    case enhancedRegular:
          // Enhanced Regular

          wr_buff[0]= BMX055_MAG_REP_XY;
          wr_buff[1]=   0x07;
          i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0); // 15 repetitions (oversampling)

          wr_buff[0]= BMX055_MAG_REP_Z;
          wr_buff[1]=   0x22;
          i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0);// 27 repetitions (oversampling)
          break;
    case highAccuracy:
          // High Accuracy
          wr_buff[0]= BMX055_MAG_REP_XY;
          wr_buff[1]=   0x17;
          i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0);// 47 repetitions (oversampling)

          wr_buff[0]= BMX055_MAG_REP_Z;
          wr_buff[1]=   0x51;
          i2c_operation(BMX055_MAG_ADDRESS,wr_buff,2,rx_buff,0);// 83 repetitions (oversampling)
          break;
  }
}

void readAccelData(uint8_t *destination)
{
    char wr_buff[10],rawData[10];

  wr_buff[0]= BMX055_ACC_D_X_LSB;
  i2c_operation(BMX055_ACC_ADDRESS,wr_buff,1,rawData,6);// Read the six raw data registers into data array

  if((rawData[0] & 0x01) && (rawData[2] & 0x01) && (rawData[4] & 0x01)) {  // Check that all 3 axes have new data
      RawSample.x.u16 = (uint16_t) (((uint16_t)rawData[1] << 8) | rawData[0]) >> 4;  // Turn the MSB and LSB into a signed 12-bit value
      RawSample.x.u16 = (uint16_t) (((uint16_t)rawData[3] << 8) | rawData[2]) >> 4;
      RawSample.x.u16 = (uint16_t) (((uint16_t)rawData[5] << 8) | rawData[4]) >> 4;
  }
  *destination++ = RawSample.x.u8.hi;
  *destination++ = RawSample.x.u8.lo;
  *destination++ = RawSample.y.u8.hi;
  *destination++ = RawSample.y.u8.lo;
  *destination++ = RawSample.z.u8.hi;
  *destination++ = RawSample.z.u8.lo;
}


void readGyroData(int16_t * destination)
{
  char wr_buff[10],rawData[10];

  wr_buff[0]= BMX055_GYRO_RATE_X_LSB;
  i2c_operation(BMX055_GYRO_ADDRESS,wr_buff,1,rawData,6);// Read the six raw data registers into data array

  destination[0] = (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]);   // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = (int16_t) (((int16_t)rawData[3] << 8) | rawData[2]);
  destination[2] = (int16_t) (((int16_t)rawData[5] << 8) | rawData[4]);
}

void readMagData(int16_t * magData)
{
  char wr_buff[10],rawData[10];
  int16_t mdata_x = 0, mdata_y = 0, mdata_z = 0, temp = 0;
  uint16_t data_r = 0;

    wr_buff[0]= BMX055_MAG_XOUT_LSB;
    i2c_operation(BMX055_MAG_ADDRESS,wr_buff,1,rawData,8);  // Read the eight raw data registers sequentially into data array


    if(rawData[6] & 0x01) { // Check if data ready status bit is set
    mdata_x = (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]) >> 3;  // 13-bit signed integer for x-axis field
    mdata_y = (int16_t) (((int16_t)rawData[3] << 8) | rawData[2]) >> 3;  // 13-bit signed integer for y-axis field
    mdata_z = (int16_t) (((int16_t)rawData[5] << 8) | rawData[4]) >> 1;  // 15-bit signed integer for z-axis field
    data_r = (uint16_t) (((uint16_t)rawData[7] << 8) | rawData[6]) >> 2;  // 14-bit unsigned integer for Hall resistance

   // calculate temperature compensated 16-bit magnetic fields
   temp = ((int16_t)(((uint16_t)((((int32_t)dig_xyz1) << 14)/(data_r != 0 ? data_r : dig_xyz1))) - ((uint16_t)0x4000)));
   magData[0] = ((int16_t)((((int32_t)mdata_x) *
        ((((((((int32_t)dig_xy2) * ((((int32_t)temp) * ((int32_t)temp)) >> 7)) +
           (((int32_t)temp) * ((int32_t)(((int16_t)dig_xy1) << 7)))) >> 9) +
         ((int32_t)0x100000)) * ((int32_t)(((int16_t)dig_x2) + ((int16_t)0xA0)))) >> 12)) >> 13)) +
      (((int16_t)dig_x1) << 3);

   temp = ((int16_t)(((uint16_t)((((int32_t)dig_xyz1) << 14)/(data_r != 0 ? data_r : dig_xyz1))) - ((uint16_t)0x4000)));
   magData[1] = ((int16_t)((((int32_t)mdata_y) *
        ((((((((int32_t)dig_xy2) * ((((int32_t)temp) * ((int32_t)temp)) >> 7)) +
           (((int32_t)temp) * ((int32_t)(((int16_t)dig_xy1) << 7)))) >> 9) +
               ((int32_t)0x100000)) * ((int32_t)(((int16_t)dig_y2) + ((int16_t)0xA0)))) >> 12)) >> 13)) +
      (((int16_t)dig_y1) << 3);
   magData[2] = (((((int32_t)(mdata_z - dig_z4)) << 15) - ((((int32_t)dig_z3) * ((int32_t)(((int16_t)data_r) -
  ((int16_t)dig_xyz1))))>>2))/(dig_z2 + ((int16_t)(((((int32_t)dig_z1) * ((((int16_t)data_r) << 1)))+(1<<15))>>16))));
    }
  }


