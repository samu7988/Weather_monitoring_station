# PES_Project_weather_monitoring_station</br>
The goal of the project is to implement a weather monitoring station.</br> 
The entire system consists of atmospheric sensor (spark fun), FRDM KL25z and a Bluetooth module.</br>
The atmospheric sensor has capability to detect temperature, pressure and humidity.</br>
The FRDM board will read the values of temperature, pressure and humidity from atmospheric sensor using SPI interface.</br>
Once the values are read by FRDM board, the values will be sent periodically to an Android application on mobile phone through Bluetooth protocol.</br>
The FRDM board is connected to Bluetooth sensor using UART protocol.</br>

### Hardware</br>
![Hardware](https://github.com/samu7988/PES_Project_weather_monitoring_station/blob/main/Hardware.jpeg)

### Sensor values sent to bluetooth application on mobile</br>
![Sensor_values_on_application](https://github.com/samu7988/PES_Project_weather_monitoring_station/blob/main/Bluetooth_values.jpeg)


### Video link for project </br>
![Project demo](https://www.youtube.com/watch?v=yTPj0NAjv9U)