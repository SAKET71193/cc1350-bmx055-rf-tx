/*
 * user_i2c.h
 *
 *  Created on: 26-Jul-2017
 *      Author: saket
 */

#ifndef USER_I2C_H_
#define USER_I2C_H_

int i2c_operation(char slave_address,char *txbuf_ptr,int tx_count, char *rxbuf_ptr,int rx_count);
void user_i2c_init(void);


#endif /* USER_I2C_H_ */
