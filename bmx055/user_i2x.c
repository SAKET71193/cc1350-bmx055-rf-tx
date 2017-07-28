/*
 * user_i2x.c
 *
 *  Created on: 26-Jul-2017
 *      Author: saket
 */
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>
#include "Board.h"
#include "user_i2c.h"
I2C_Handle      i2c;
I2C_Params      i2cParams;
I2C_Transaction i2cTransaction;
extern Display_Handle display;

void user_i2c_init()
{

        I2C_Params_init(&i2cParams);
        i2cParams.bitRate = I2C_400kHz;
        i2c = I2C_open(Board_I2C_TMP, &i2cParams);
        if (i2c == NULL) {
            Display_printf(display, 0, 0, "Error Initializing I2C\n");
            while (1);
        }
        else {
            Display_printf(display, 0, 0, "I2C Initialized!\n");
        }
}


int i2c_operation(char slave_address,char *txbuf_ptr,int tx_count, char *rxbuf_ptr,int rx_count)
{
    int reg;


           i2cTransaction.slaveAddress = slave_address;
           i2cTransaction.writeBuf = txbuf_ptr;
           i2cTransaction.writeCount = tx_count;
           i2cTransaction.readBuf = rxbuf_ptr;
           i2cTransaction.readCount = rx_count;

               if (I2C_transfer(i2c, &i2cTransaction)) {
                   Display_printf(display, 0, 0, "I2C transaction done \n");
                      reg = 0;
               }
               else{
                   reg = 1;
                   Display_printf(display, 0, 0, "I2C Bus fault\n");
               }
return reg;
}
