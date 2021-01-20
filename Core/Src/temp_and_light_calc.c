#include "temp_and_light_calc.h"

uint16_t calculate_temp(uint16_t temp_mes) {
		//const values for ADC temp calculations
		const float V25 = 0.76; 			//[Volts]
		const float Avrg_Slope = 0.0025; 	//[Volts/degree]
		const float Supply_Voltage = 3.3;  	//[Volts]
		const float ADC_Resolution = 4096.0;

		//variables for ADC temp calculations
		float temperature;
		float Vsense;

		Vsense = Supply_Voltage * temp_mes / ADC_Resolution;
		temperature = ((Vsense - V25) / Avrg_Slope) + 25;

		return (uint16_t) (temperature*10);
}

int calculate_light(int light_mes, float min, float max) {
	// Calculate light percentage based on configured values
	float a = 100.0/(max - min);
	float b = a*(-min);

	int light = (int)(a * light_mes + b);

	if (light < 0) {return 0;}

	return light;
}
