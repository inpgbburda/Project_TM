#ifndef INC_TEMP_AND_LIGHT_CALC_H_

#define INC_TEMP_AND_LIGHT_CALC_H_

#include "stm32f4xx_hal.h"
uint16_t calculate_temp(uint16_t temp_mes);
int calculate_light(int light_mes, float min, float max);

#endif /* INC_TEMP_AND_LIGHT_CALC_H_ */
