/*
 * LPS25HB.c
 *
 *  Created on: 28 gru 2020
 *      Author: Bartłomiej Burda
 */
#include "i2c.h"
#include "LPS25HB.h"

void LPS25HB_Init (void){
	uint8_t Settings = LPS25HB_PRESS_25HZ | LPS25HB_PD_ON;
	HAL_I2C_Mem_Write(&hi2c2, LPS25HB_ADDR, LPS25HB_CTRL_REG1, 1, &Settings, 1, 100);
}

int LPS25HB_Read_Press(void){
	uint8_t Received_Data[3];
	uint32_t Press_RAW;

	HAL_I2C_Mem_Read(&hi2c2, LPS25HB_ADDR, LPS25HB_PRESS_OUT_XL_MULTI_READ, 1, Received_Data, 3, 100);
	Press_RAW = (Received_Data[2] << 16) | (Received_Data[1] << 8) | Received_Data[0];
	return Press_RAW / 4096;
}
