/*
 * LPS25HB.h
 *
 *  Created on: 28 gru 2020
 *      Author: Bartłomiej Burda
 */

#ifndef INC_LPS25HB_H_
#include "stm32f4xx_hal.h"

//register definitions
#define LPS25HB_ADDR		(0b1011100 << 1)  // address SAD of sensor equal 1011100x (SAD[0]=0)
#define WHO_AM_I_REG 		0x0F
#define LPS25HB_CTRL_REG1 	0x20 //settings register 1
#define PRESS_OUT_XL 		0x28 //the lowest byte of raw pressure data
#define PRESS_OUT_L 		0x29 //middle byte of raw pressure data
#define PRESS_OUT_H 		0x2A //the highest byte of raw pressure data

#define LPS25HB_PRESS_OUT_XL_MULTI_READ (PRESS_OUT_XL| 0x80)

//CTRL_REG1 = [PD] [ODR2] [ODR1] [ODR0] [DIFF_EN] [BDU] [RESET_AZ] [SIM]
#define LPS25HB_PRESS_25HZ 0x40 // 0100 0000 0b
#define LPS25HB_PD_ON 0x80 		// 1000 000 0b


//sensor functions declarations
void LPS25HB_Init (void); 		// sensor initialization

int LPS25HB_Read_Press(void); 	// polling mode reading pressure

#define INC_LPS25HB_H_



#endif /* INC_LPS25HB_H_ */
