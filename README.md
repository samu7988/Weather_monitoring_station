# PES_Project_weather_monitoring_station</br>
The goal of the project is to implement a weather monitoring station.</br> 
The entire system consists of atmospheric sensor (spark fun), FRDM KL25z and a Bluetooth module.</br>
The atmospheric sensor has capability to detect temperature, pressure and humidity.</br>
The FRDM board will read the values of temperature, pressure and humidity from atmospheric sensor using SPI interface.</br>
Once the values are read by FRDM board, the values will be sent periodically to an Android application on mobile phone through Bluetooth protocol.</br>
The FRDM board is connected to Bluetooth sensor using UART protocol.