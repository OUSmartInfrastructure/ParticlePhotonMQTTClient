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

		

## Setting up the AWS IoT Core service
Amazon usually requires that you first setup an account with them. Once the account has been setup, head over to the [IoT Core](https://aws.amazon.com/iot-core/) site and log into the console.

In the console, search for the IoT Core service and select it to get into the IoT Core service's console. You should note that Amazon is constantly evolving their platform so moving forward if you are accessing this resource at some time later than 2018 there may be little changes but it shouldn't be hard to figure things out.

The following steps are also available via a youtube video.

 1. On the left menu navigation pane, select 'Manage'
 2. Then select 'Things'. If you have no Thing created, you will be prompted to create one if not, click on 'Create' to create one.
 3. Since we will be working with just one thing, which is out Photon+BME280 bundle, select 'Create a Single Thing'
 4. Give your thing a very cool name. In this instance, I called mine 'MyFirstThing'
 5. Besides the name, leave everything else as their defaults and click select 'Next'
 6. The next screen prompts you to add a certificate (I'll elaborate some more about this step later). Select 'One-click certificate creation (recommended)'
 7. On the next screen, you will be presented with four downloadable items. Two key files and two certificate files, be sure to download each of them and save them some place you can easily remember
 8. Then select 'Activate' to activate your new certificate
 9. On the same screen, select 'Attach a Policy'
 10. If no policies are present, you will be prompted to create one. Give your policy a cool name like 'Policy_for_MyFirstThing' like I did. Under action, select 'iot:&ast;' and under Resource ARN, provide '&ast;'. Then under effect, check 'Allow' and save the policy
 11. Navigate back to the main menu and select 'Certificates' under 'Secure'. For the active certificate you just created, click on the tri-dot menu popup and first attach a policy and then attach your thing. Both should be what you just created.
 12. 
 
