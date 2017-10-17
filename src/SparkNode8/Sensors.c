/*
 * Sensors.c
 *
 * Created: 2017-08-17 14:43:14
 *  Author: etudiant
 */ 

 #include "hardware.h"
 #include "Sensors.h"
 #include "sensor_timer.h"
 #include "OPT3001.h"
 #include "Si7021.h"

 uint8_t sensor_data[7];
 uint8_t* light_data;
 uint8_t* temp_data;
 uint8_t* RH_data;

uint8_t* Sensors_get(void)
{

	//light sensor data
	light_data = OPT3001_read();
	sensor_data[2] = *light_data;
	sensor_data[3] = *(light_data+1);

	//Measure temperature
	Si7021_command(SI7021_MEAS_T_HMH);
	sensor_timer_Delay(20);
	temp_data = Si7021_read();
	sensor_data[0] = *temp_data;
	sensor_data[1] = *(temp_data+1);

	//Measure Relative humidity
	Si7021_command(SI7021_MEAS_RH_HMH);
	sensor_timer_Delay(20);
	RH_data = Si7021_read();
	sensor_data[4] = *RH_data;
	sensor_data[5] = *(RH_data+1);
	sensor_data[6] = 0xc3;

	return &sensor_data;

}