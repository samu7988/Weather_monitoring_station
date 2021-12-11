
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
	SPI_read_register(BME280_DIG_T1_MSB_REG, &T1_LSB);
	T1 = (T1_MSB << 8) | (T1_LSB);

	uint16_t T2_MSB = 0;
	uint16_t T2_LSB = 0;
	uint16_t T2 = 0;
	SPI_read_register(BME280_DIG_T2_MSB_REG, &T2_MSB);
	SPI_read_register(BME280_DIG_T2_MSB_REG, &T2_LSB);
	T2 = (T2_MSB << 8) | (T2_LSB);

	uint16_t T3_MSB = 0;
	uint16_t T3_LSB = 0;
	uint16_t T3 = 0;
	SPI_read_register(BME280_DIG_T3_MSB_REG, &T3_MSB);
	SPI_read_register(BME280_DIG_T3_MSB_REG, &T3_LSB);
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

float read_temp_f( void )
{
	float output = read_temp_C();
	output = (output * 9) / 5 + 32;

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
