
/***********************************************************************************
* @file bme280.c
 * @brief:BME280 related functions
 * @author Sayali Mule
 * @date 12/04/2021
 * @Reference:
 *
 *****************************************************************************/
//***********************************************************************************
//                              Include files
//***********************************************************************************
#include "MKL25Z4.h"
#include "spi.h"
#include "bme280.h"
#include "uart.h"

//***********************************************************************************
//                                  Macros
//***********************************************************************************

//***********************************************************************************
//                              Structures
//***********************************************************************************
int32_t t_fine = 0;


//***********************************************************************************
//                                  Function definition
//***********************************************************************************
uint8_t bme280_init()
{
	uint8_t standby = 0; //0.5ms
	uint8_t filter = 0; //Filter off
	uint8_t temp_over_sample = 1;
	uint8_t press_over_sample = 1;
	uint8_t humid_over_sample = 1;
//
//	//Most of the time the sensor will be init with default values
//	//But in case user has old/deprecated code, use the settings.x values
	uint8_t read_data = 0;
	set_standby_time(standby);
	set_filter(filter);
	set_pressure_oversample(press_over_sample); //Default of 1x oversample
	set_humidity_oversample(humid_over_sample); //Default of 1x oversample
	set_temp_oversample(temp_over_sample); //Default of 1x oversample
//
	set_mode(MODE_NORMAL); //Go!


	SPI_read_register(BME280_CHIP_ID_REG, &read_data);
	return read_data; //Should return 0x60
}

//Set the standby bits in the config register
//tStandby can be:
//  0, 0.5ms
//  1, 62.5ms
//  2, 125ms
//  3, 250ms
//  4, 500ms
//  5, 1000ms
//  6, 10ms
//  7, 20ms
void set_standby_time(uint8_t timeSetting)
{
	if(timeSetting > 0x7) timeSetting = 0; //Error check. Default to 0.5ms

	uint8_t control_data = 0;
	SPI_read_register(BME280_CONFIG_REG,&control_data);
	control_data &= ~( (1<<7) | (1<<6) | (1<<5) ); //Clear the 7/6/5 bits
	control_data |= (timeSetting << 5); //Align with bits 7/6/5
	SPI_write_register(BME280_CONFIG_REG, control_data);
	SPI_read_register(BME280_CONFIG_REG,&control_data);
	int x = 0;
}

//Set the filter bits in the config register
//filter can be off or number of FIR coefficients to use:
//  0, filter off
//  1, coefficients = 2
//  2, coefficients = 4
//  3, coefficients = 8
//  4, coefficients = 16
void set_filter(uint8_t filter_setting)
{
	if(filter_setting > 0x07) filter_setting = 0; //Error check. Default to filter off

	uint8_t control_data = 0;
	SPI_read_register(BME280_CONFIG_REG,&control_data);
	control_data &= ~( (1<<4) | (1<<3) | (1<<2) ); //Clear the 4/3/2 bits
	control_data |= (filter_setting << 2); //Align with bits 4/3/2
	SPI_write_register(BME280_CONFIG_REG, control_data);
}

//Set the temperature oversample value
//0 turns off temp sensing
//1 to 16 are valid over sampling values
void set_temp_oversample(uint8_t over_sample_amount)
{
	over_sample_amount = check_sample_value(over_sample_amount); //Error check

	uint8_t originalMode = get_mode(); //Get the current mode so we can go back to it at the end

	set_mode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_t bits (7, 6, 5) to overSampleAmount
	uint8_t control_data = 0;
	SPI_read_register(BME280_CTRL_MEAS_REG,&control_data);

	control_data &= ~( (1<<7) | (1<<6) | (1<<5) ); //Clear bits 765
	control_data |= over_sample_amount << 5; //Align overSampleAmount to bits 7/6/5
	SPI_write_register(BME280_CTRL_MEAS_REG, control_data);


	set_mode(originalMode); //Return to the original user's choice
}

//Validates an over sample value
//Allowed values are 0 to 16
//These are used in the humidty, pressure, and temp oversample functions
uint8_t check_sample_value(uint8_t user_value)
{
	switch(user_value)
	{
		case(0):
			return 0;
			break; //Valid
		case(1):
			return 1;
			break; //Valid
		case(2):
			return 2;
			break; //Valid
		case(4):
			return 3;
			break; //Valid
		case(8):
			return 4;
			break; //Valid
		case(16):
			return 5;
			break; //Valid
		default:
			return 1; //Default to 1x
			break; //Good
	}
}

//Gets the current mode bits in the ctrl_meas register
//Mode 00 = Sleep
// 01 and 10 = Forced
// 11 = Normal mode
uint8_t get_mode()
{
	uint8_t control_data = 0;
	SPI_read_register(BME280_CTRL_MEAS_REG,&control_data);

	return(control_data & 0x03); //Clear bits 7 through 2
}

//Set the mode bits in the ctrl_meas register
// Mode 00 = Sleep
// 01 and 10 = Forced
// 11 = Normal mode
void set_mode(uint8_t mode)
{
	if(mode > 0x3) mode = 0; //Error check. Default to sleep mode

	uint8_t control_data = 0;
	SPI_read_register(BME280_CTRL_MEAS_REG,&control_data);
	control_data &= ~( (1<<1) | (1<<0) ); //Clear the mode[1:0] bits
	control_data |= mode; //Set
	SPI_write_register(BME280_CTRL_MEAS_REG, control_data);
}

//Set the pressure oversample value
//0 turns off pressure sensing
//1 to 16 are valid over sampling values
void set_pressure_oversample(uint8_t over_sample_amount)
{
	over_sample_amount = check_sample_value(over_sample_amount); //Error check

	uint8_t original_mode = get_mode(); //Get the current mode so we can go back to it at the end

	set_mode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_p bits (4, 3, 2) to overSampleAmount
	uint8_t control_data = 0;
	SPI_read_register(BME280_CTRL_MEAS_REG,&control_data);

	control_data &= ~( (1<<4) | (1<<3) | (1<<2) ); //Clear bits 432
	control_data |= over_sample_amount << 2; //Align overSampleAmount to bits 4/3/2
	SPI_write_register(BME280_CTRL_MEAS_REG, control_data);

	set_mode(original_mode); //Return to the original user's choice
}

//Set the humidity oversample value
//0 turns off humidity sensing
//1 to 16 are valid over sampling values
void set_humidity_oversample(uint8_t over_sample_amount)
{
	over_sample_amount = check_sample_value(over_sample_amount); //Error check

	uint8_t original_mode = get_mode(); //Get the current mode so we can go back to it at the end

	set_mode(MODE_SLEEP); //Config will only be writeable in sleep mode, so first go to sleep mode

	//Set the osrs_h bits (2, 1, 0) to overSampleAmount
	uint8_t control_data = 0;
	SPI_read_register(BME280_CTRL_HUMIDITY_REG,&control_data);

	control_data &= ~( (1<<2) | (1<<1) | (1<<0) ); //Clear bits 2/1/0
	control_data |= over_sample_amount << 0; //Align overSampleAmount to bits 2/1/0
	SPI_write_register(BME280_CTRL_HUMIDITY_REG, control_data);

	set_mode(original_mode); //Return to the original user's choice
}

float read_temp_C( void )
{
	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value

	//get the reading (adc_T);
    uint8_t buffer[3];

    SPI_read_register(BME280_TEMPERATURE_MSB_REG, &buffer[0]);
    SPI_read_register(BME280_TEMPERATURE_LSB_REG, &buffer[1]);
    SPI_read_register(BME280_TEMPERATURE_XLSB_REG, &buffer[2]);
    int32_t adc_T = ((uint32_t)buffer[0] << 12) | ((uint32_t)buffer[1] << 4) | ((buffer[2] >> 4) & 0x0F);


	uint16_t T1_MSB = 0;
	uint16_t T1_LSB = 0;
	uint16_t T1 = 0;
	SPI_read_register(BME280_DIG_T1_MSB_REG, &T1_MSB);
	SPI_read_register(BME280_DIG_T1_LSB_REG, &T1_LSB);
	T1 = (T1_MSB << 8) | (T1_LSB);

	uint16_t T2_MSB = 0;
	uint16_t T2_LSB = 0;
	uint16_t T2 = 0;
	SPI_read_register(BME280_DIG_T2_MSB_REG, &T2_MSB);
	SPI_read_register(BME280_DIG_T2_LSB_REG, &T2_LSB);
	T2 = (T2_MSB << 8) | (T2_LSB);

	uint16_t T3_MSB = 0;
	uint16_t T3_LSB = 0;
	uint16_t T3 = 0;
	SPI_read_register(BME280_DIG_T3_MSB_REG, &T3_MSB);
	SPI_read_register(BME280_DIG_T3_LSB_REG, &T3_LSB);
	T3 = (T3_MSB << 8) | (T3_LSB);

	//By datas																																																				heet, calibrate
	int64_t var1, var2;
	var1 = ((((adc_T>>3) - ((int32_t)T1<<1))) * ((int32_t)T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)T1)) * ((adc_T>>4) - ((int32_t)T1))) >> 12) *
	((int32_t)T3)) >> 14;
	t_fine = var1 + var2;
	float output = (t_fine * 5 + 128) >> 8;

	float temp_correction = 0.f;
	output = output / 100 + temp_correction;

	return output;
}

float read_float_humidity( void )
{

	// Returns humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
	// Output value of “47445” represents 47445/1024 = 46. 333 %RH
    uint8_t buffer[2];
	SPI_read_register(BME280_HUMIDITY_MSB_REG, &buffer[0]);
	SPI_read_register(BME280_HUMIDITY_LSB_REG, &buffer[1]);

    int32_t adc_H = ((uint32_t)buffer[0] << 8) | ((uint32_t)buffer[1]);

	int32_t var1;
	var1 = (t_fine - ((int32_t)76800));


	uint8_t H1 = 0;
	SPI_read_register(BME280_DIG_H1_REG, &H1);

	uint16_t H2_MSB = 0;
	uint16_t H2_LSB = 0;
	uint16_t H2 = 0;
	SPI_read_register(BME280_DIG_H2_MSB_REG, &H2_MSB);
	SPI_read_register(BME280_DIG_H2_LSB_REG, &H2_LSB);
	H2 = (H2_MSB << 8) | (H2_LSB);

	uint8_t H3 = 0;
	SPI_read_register(BME280_DIG_H3_REG, &H3);

	uint16_t H4_MSB = 0;
	uint16_t H4_LSB = 0;
	uint16_t H4 = 0;
	SPI_read_register(BME280_DIG_H4_MSB_REG, &H4_MSB);
	SPI_read_register(BME280_DIG_H4_LSB_REG, &H4_LSB);
	H4 = (H4_MSB << 4) | (H4_LSB & 0x0F);

	uint16_t H5_MSB = 0;
	uint16_t H5_LSB = 0;
	uint16_t H5 = 0;
	SPI_read_register(BME280_DIG_H5_MSB_REG, &H5_MSB);
	SPI_read_register(BME280_DIG_H4_LSB_REG, &H5_LSB);
	H5 = (H5_MSB << 4) | ((H5_LSB >> 4) & 0x0F);

	uint8_t H6 = 0;
	SPI_read_register(BME280_DIG_H6_REG, &H6);



	var1 = (((((adc_H << 14) - (((int32_t)H4) << 20) - (((int32_t)H5) * var1)) +
	((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)H6)) >> 10) * (((var1 * ((int32_t)H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
	((int32_t)H2) + 8192) >> 14));

	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)H1)) >> 4));
	var1 = (var1 < 0 ? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);

	float output = (float)(var1>>12)/1024.0;
	return output;
}

float readFloatPressure( void )
{

	// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
	// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
    uint8_t buffer[3];
	SPI_read_register(BME280_PRESSURE_MSB_REG, &buffer[0]);
	SPI_read_register(BME280_PRESSURE_LSB_REG, &buffer[1]);
	SPI_read_register(BME280_PRESSURE_XLSB_REG, &buffer[2]);

    int32_t adc_P = ((uint32_t)buffer[0] << 12) | ((uint32_t)buffer[1] << 4) | ((buffer[2] >> 4) & 0x0F);

	int64_t var1, var2, p_acc;
	var1 = ((int64_t)t_fine) - 128000;

	uint16_t P1_MSB = 0;
	uint16_t P1_LSB = 0;
	uint16_t P1 = 0;
	SPI_read_register(BME280_DIG_P1_MSB_REG, &P1_MSB);
	SPI_read_register(BME280_DIG_P1_LSB_REG, &P1_LSB);
	P1 = (P1_MSB << 8) | (P1_LSB);

	uint16_t P2_MSB = 0;
	uint16_t P2_LSB = 0;
	uint16_t P2 = 0;
	SPI_read_register(BME280_DIG_P2_MSB_REG, &P2_MSB);
	SPI_read_register(BME280_DIG_P2_LSB_REG, &P2_LSB);
	P2 = (P2_MSB << 8) | (P2_LSB);

	uint16_t P3_MSB = 0;
	uint16_t P3_LSB = 0;
	uint16_t P3 = 0;
	SPI_read_register(BME280_DIG_P3_MSB_REG, &P3_MSB);
	SPI_read_register(BME280_DIG_P3_LSB_REG, &P3_LSB);
	P3 = (P3_MSB << 8) | (P3_LSB);

	uint16_t P4_MSB = 0;
	uint16_t P4_LSB = 0;
	uint16_t P4 = 0;
	SPI_read_register(BME280_DIG_P4_MSB_REG, &P4_MSB);
	SPI_read_register(BME280_DIG_P4_LSB_REG, &P4_LSB);
	P4 = (P4_MSB << 8) | (P4_LSB);

	uint16_t P5_MSB = 0;
	uint16_t P5_LSB = 0;
	uint16_t P5 = 0;
	SPI_read_register(BME280_DIG_P5_MSB_REG, &P5_MSB);
	SPI_read_register(BME280_DIG_P5_LSB_REG, &P5_LSB);
	P5 = (P5_MSB << 8) | (P5_LSB);

	uint16_t P6_MSB = 0;
	uint16_t P6_LSB = 0;
	uint16_t P6 = 0;
	SPI_read_register(BME280_DIG_P6_MSB_REG, &P6_MSB);
	SPI_read_register(BME280_DIG_P6_LSB_REG, &P6_LSB);
	P6 = (P6_MSB << 8) | (P6_LSB);

	uint16_t P7_MSB = 0;
	uint16_t P7_LSB = 0;
	uint16_t P7 = 0;
	SPI_read_register(BME280_DIG_P7_MSB_REG, &P7_MSB);
	SPI_read_register(BME280_DIG_P7_LSB_REG, &P7_LSB);
	P7 = (P7_MSB << 8) | (P7_LSB);

	uint16_t P8_MSB = 0;
	uint16_t P8_LSB = 0;
	uint16_t P8 = 0;
	SPI_read_register(BME280_DIG_P8_MSB_REG, &P8_MSB);
	SPI_read_register(BME280_DIG_P8_LSB_REG, &P8_LSB);
	P8 = (P8_MSB << 8) | (P8_LSB);

	uint16_t P9_MSB = 0;
	uint16_t P9_LSB = 0;
	uint16_t P9 = 0;
	SPI_read_register(BME280_DIG_P9_MSB_REG, &P9_MSB);
	SPI_read_register(BME280_DIG_P9_LSB_REG, &P9_LSB);
	P9 = (P9_MSB << 8) | (P9_LSB);

	var2 = var1 * var1 * (int64_t)P6;
	var2 = var2 + ((var1 * (int64_t)P5)<<17);
	var2 = var2 + (((int64_t)P4)<<35);
	var1 = ((var1 * var1 * (int64_t)P3)>>8) + ((var1 * (int64_t)P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p_acc = 1048576 - adc_P;
	p_acc = (((p_acc<<31) - var2)*3125)/var1;
	var1 = (((int64_t)P9) * (p_acc>>13) * (p_acc>>13)) >> 25;
	var2 = (((int64_t)P8) * p_acc) >> 19;
	p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)P7)<<4);

	float output = (float)p_acc / 256.0;
	return output;

}

void read_sensors(sensor_val_t* sensor_val)
{
 //TODO: read the values of temperature, humidity and pressure
	sensor_val->temp_val = 25;
	sensor_val->pressure_val = 5;
	sensor_val->hum_val = 7;
}

void transmit_sensors_val(sensor_val_t* sensor_val)
{
	//TODO: Send value of sensors via Bluetooth to mobile application
	uart1_puts((uint8_t*)"T:\n");
	uart1_puts((uint8_t*)"P:\n");
	uart1_puts((uint8_t*)"H:\n");
}
