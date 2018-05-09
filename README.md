# Using Particle's Photon as a MQTT client

[MQTT](http://mqtt.org/) is a machine-to-machine (M2M)/"Internet of Things" connectivity protocol. It was designed as an extremely lightweight publish/subscribe messaging transport. It is designed for constrained devices and low-bandwidth, high-latency or unreliable networks.

Particle's [Photon](https://www.particle.io/products/hardware/photon-wifi/) is small and powerful wifi-connected micontroller capable of basic to advaced telemetry when interfaced with sensor units. To demonstrate the use of Photon as an MQTT client, the microcontroller was interfaced with the Bosch Sensortec Intergrated Environmental Sensor Unit _BME280_. To make the unit readily accessible, a breakout version from [Adafruit](https://www.adafruit.com/product/2652) was obtained. 

The scope of this project comprised setting up the microcontroller as a MQTT client, to report telemetry data from the Environmental Sensor Unit to the [Amazon AWS IoT Core](https://aws.amazon.com/iot-core/) service. AWS IoT Core is a managed cloud platform that lets connected devices easily and securely interact with cloud applications and other devices via MQTT/TLS. Once the data reached the service, the secondary objective was to write the data to the [DynamoDB](https://aws.amazon.com/dynamodb/) time-series database which is another AWS service.

# Getting Started
To get things started, it was required to:
 - Obtain a [Photon](https://www.particle.io/products/hardware/photon-wifi/)
 - A breadboard and a couple of jumpers
 - The [BME280](https://www.adafruit.com/product/2652) sensor unit
 - An Amazon AWS account 
 - [MQTT fx](http://mqttfx.jensd.de/index.php/download) for testing purposes

## Setting up the Photon with the sensor unit
Once the photon has been [setup](https://docs.particle.io/guide/getting-started/start/photon/) and the development environment is ready for use, you can go on integrate the environmental sensor. The sensor supports both I2C and SPI communications but for this project, I2C was used and the only advantage in doing this had to do with the number of pins required (2 instead of 4) for communications with the Photon.

![Photon and sensor setup](https://lh3.googleusercontent.com/TCk_xNIiJ_JGC0zZXulXLKgJJQP_RB0OIq0QRRNtOWTtSb2X17bn9YIvIbSZZzYKyhySkzzh4R_vSAX-vHgD_lu7LtXR9AldzJ9YpsqClJ1qEmZdYEN_FyHTIZuhF7v_cZ5XaEHc5BOthVijAeXEF2UCp1wJGjAhlryThdGaRHkvtQcIrf-L93gO8GMqlFufHrs7C3m_yUGoFn-RRpFlaY64naZ8TvlqA44u0oizG-zjK1H6MuQUi1SJR09_cOVJQrJII8AhKwxxOmzWB0wKWu5noStk-x74D6HtcC6hNWab-GA_M-Lt-JInyLckOKhzAat1eXp66uvM7n0PFpbDamBOV0mBORC09Yo5UjWKINDukFqWWGOT55WeOKMefXQBrFob7sfvm21n9hHyWAOFtVMEOtio16uihyejzZwp9hLXnZKhV8guQ-iy3fa-xIJvxruMjxh1XF4aHw4mi_8rX3JRv9naspHjshj1Cod6qzVQLB4QRm4VaW8lj4T_DyRY5zwZ-q2-1XBBjGQFZFkb5PtmexebbXOTvx_RmVj14ez4WFBLiMNW64USn_WTMMtVUBzlTDZC84rqnaNjArRVckBErT1A_6jZUaKa5xvs=w705-h436-no)

A slightly modified (for simplicity) version of the library by Thomas Rockenbauer (https://github.com/hl68fx) who adapted the original library (for Arduino) to work with Particle Photon & Core was used.

By default, the sensor should be addressable via 0x76, but there have been some cases where it was only accesible via 0x77. Should you run into such problems be sure to first run the adress scan sketch also included in this project in the examples directory. The file to look for is i2cscan.ino and all this will do is to scan the entire I2C addressable block and report which ones have something connected via the serial monitor so be sure to setup that one too.

To test the sensor exclusively, you can run the sketch below in your web IDE and be sure to lookout for the readings in your serial monitor.

    #include "BME280.h"
    
    BME280 myBME;
    
    void setup()
    {
	    // Initialise I2C communication as MASTER
	    Wire.begin();
	    // Initialise Serial communication, set baud rate = 9600
	    Serial.begin(9600);
	}
	void loop()
	{
		myBME.bme280_init();//init sensor
	    double cTemp = myBME.bme280_getCtemp();//get celcius temperature
	    getTempColor(cTemp);
	    Serial.println(cTemp);
	}

		
