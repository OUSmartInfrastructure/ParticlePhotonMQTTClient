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
 12. Back at the main menu, select Manage and then select Things. Under your thing, select Interact to obtain the Rest API Endpoint for your thing
 13. Your thing should be setup now, we will test it in the next steps
 
 
 
## Testing connectivity to your 'Thing'

The next requires that you download an MQTT client to test the credentials provided. Amazon uses some of the four files you downloaded to authenticate you as an authoritarian over the thing you are trying to access as well as for authenticating the TLS communications with its servers.

The MQTT client of choice is [MQTT fx](http://mqttfx.jensd.de/index.php/download). The link to download this was also provided under the requirements section.

Once it has been downloaded, start it and create a new connection profile. Your profile should be setup to look like this.
![MQTT fx Connection Profile](https://lh3.googleusercontent.com/QAByr3t5FToos3TXXjI7jkWPoYZLwVHiUZfc7EWfJCLrnk5RG5MaAVzDHKxBhMZvb2WO4qVswrtQXpZatFg6PwsZ_ZBYvNJdtry-xjaDO_WNXoSpzRRTnkcqVqrly3ljSbIY34jcrxDQB5JVUnRQ7NxRExvTATmvX6bosuXyqT4cIQtWkMWAzHPl1u78_rvO5WsVtX_7gvxB7jbUwiI5VAFFiaG6F8XsD1g0qqD8seDfh6sXyPnH4uBayB8C4xNIEIcIi13bgmmUJErW5LbgpG6jbVGMwZOBI0Q7l2x3jfwIsyscJflBql2E44a-NZUnkMqunLlNLYbOOx7CZ-pbUHj6_EXsILO3jhSZV20YHwOfvaeGV8fh7OANDGkv0Ji-Rd7bxBFhQZ9aRoMM-jV7cbSUb1STXePVvF-LOinXFCOjyMfDEjXFmeZrBuo-TiWjNs7ybC9ahabzxSu2Jl_hU0t4ON-7qCB-rqNxHzxeFNwjmPflEA8h0DGC0xsBXN_Aoim5kBnj8F2zWar9Foy0KKq-kFYqunLlzURXJH4gRThXbGBuIianbMpaHlwjhPTugU_e3wib2a_4AaraadON6qbTTyYHPKsh_iVwpu2w=w707-h709-no)

The CA file and Client Certificate files should be easily identifiable. If you missed the CA file, you can also obtain one from [here](//https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html). The client key file is the same as your private key. Go on to save the profile.

From the main screen of the MQTT fx client, select the profile you just created and click connect. You should see the round grey indicator in the top-right section turn green indicating a successful connection.

It should look like this.
![MQTT fx in connected mode](https://lh3.googleusercontent.com/EL6zGXfKd6B6wPUz5HxOBeSwyGswqm8pxvXLvZ_O8f721YwdrlrPbuqO0TZqZaATKl1LJQIUpyNDF1-x94QlnG3dvgWtlgADItQgsjt8tXJiZedRu35_SgTRzKjvopaQ8cNWklYU191caUibpVAUsV8rPTEMwSZheSsmISzARuBQBTqGvxdCchypW3tcRW00h5g4te4W9HklIGOnA9H7RSCKmR7x6DH3PEGUW35PpCstk8EuOHq6KGOk_g8a1LtMLVILDbddybpj1T3MKtLbbfjtfBdZUuFat_zbUDFPXm9Ge6rRu3oA-qDAW1eFWqMKavVGFELGA3J6CKzv7Nlg0N7YO5LBG-UunVifPxPINNR-I6mjlUJGFrO5OmtYHPshdive1u1UXnJKyhUmF_A9Oo_8fdP4eFLQaX6v3pdExbnPhff5SNWdbXHjtgSrLwWU6YFWDeiNkNd2WkGYMv-IxOGzVkaVC4ozn-s9CGcJfdAvma4QOqhMdbdr-sZugK4o_HsAdcua34F4uDjdmPLY9GbFzt2J4zWiY8W6Qjg9lV5HWVKzsaF0vHXNUo0y6aaiaUW7CHiIABkj2KWFcaSODDGqLMMN4d7jdfy91iwy=w1003-h223-no)

Now, head back to your IoT Core home page and let's have some fun!
In the left navigation pane, locate and select 'Test'. It should start the IoT core's inbuilt MQTT client also for testing purposes. What we are going tp do now is to publish to some topic from MQTT fx and subscribe to the same topic inside the inbuilt MQTT client.

For this demonstration, the topic selected is 'myfirstthing/test' so go ahead and publish to this topic inside MQTT fx and wait for the respose inside the subscribed section of the inbuilt MQTT client.

![MQTT fx test publish](https://lh3.googleusercontent.com/a6W-owfzYGWO5YR1l4GO6UxYyUYKG0yUjbI_cKDV8hlh9n1h8ueMOST4cCwc1LzzA7Aymv0OuXzqNdvuv0RX9oeIb9yr0oowQB7uYQT_kwEYM7miOu0L8cZdqKDQiIIZ-727l8ahlGeDNmQeW_gSihsicDqcLboqO8D-Yq7bWoykI9CrTbhQCorKV7Zb2g1MqZ-QW5u2B020S2-HdcUXMVKg3GTs8AeQjk3hVgu8G8esXeOnJhq8OyiwTkr4n2I0RU_5mtVods9uJGKw54uaXR1gSvEYEWZkflzoDBeCOo9xOOs6RlXI7BXzSZNMV4CugiJcD5UmOesArXW6TKsGXfkZSCr1zhZE0VG8A9JWF71ZHS-4iwJDlKaBWoPm3caNj9Ekdc-o5wenpgZ3OqtE3K6R5qUbZCMI2IgXdKf8kZdV6ZQBvSXl1h0DYV3Z1DVx-j_hRtiz5boW5djVYqSFcb-d709LIFmLMp9I2EAsXMkAPHP-oN4sivjOwMXTgYCjbNOuZEmnVeUcuaAFUxbrZI1JQrggW22jhoSzO0l8G_DBjd0RFUIZZTrjHpHRsWsINBcQiJ_jjKD-OgmCJg7Om0AeQGQoI5PPgW6mWYSZ=w1002-h271-no)

Subscription results
![Subscription results](https://lh3.googleusercontent.com/6LuTG8KR3kRrnz3Oy-Hz4cahcXepPcJa9pcGbLxpkDgahldZcLkVEfh32arbAYUK-eBY1Zvv_pRCnqwJBrBLCilGzPcU-Q-sAiKZmHt7NR73nb1jZbpJjvwR1nibiHwEeyRS3kLXCvijwqFkbWq_1n4qhJmIkCkvrm9nYwfksfn9-4aJplI6FZFgpoR9WAnvysnDc_2rkTlPR3v0Zl4w_9k1JS32UBMAAEAzLp043tXavfLgnW-cKGXvnr-5F0SrczUKQaIHZVFUcDModo37ntROpLXmeJgzMMsIa5zUYZAcHxkOLwOCO1c55sfuz6_5eAUK8x9Rahk9-rCv_Dj2BIkfW2hG5_MtihA0g4cVVam4-p1GioAblljd0QMIeJQupARlz0Qx1xp3NvSgGr6CH82PE3c4F1gdtpmZ2F3wU2_UN4-BgFaij5ZN8UrgL9ke1Mt-7AppvVoNHGkzVAkDa3jbRO9Ij_m-jghVyZrjdcekaK0keY_bIUgknHjMo4LFhPusyo-6C3ZrhFIrVLj5aISVq2fg9hxQhQdgFdltoIYSYzpL-rYCTARJCam1mKGQQQZdXFQg1z7VZhN1k124I9bZirayx3QCmAEktcQ9=w1510-h752-no)

You can go ahead to play around with these tools some more but after this step, you are assured the credentials work and can proceed to implement same with your photon. 

If you encountered any errors, you can go back and review the steps for setting up the IoT Core service or post an issue to this project.


# Implementing the MQTT library for the Photon

From my research into the beginnings of the MQTT library for the Arduino and now the Particle Core platforms, I would like to credit [Nicholas O'Leary](https://knolleary.net/) and [Hirotaka Niisato](https://github.com/hirotakaster/MQTT-TLS) for their work based on which this project was done. Hirotaka has been doing well to update the library and as at the time of writing this, the most recent was on Mar 25 2018.

To test the library, first you need to provide the content of the certificates and private key you used for testing connectivity using MQTT fx into the sample sketch below.

    #include "MQTT-TLS.h"

	void callback(char* topic, byte* payload, unsigned int length);

	#define AMAZON_IOT_ROOT_CA_PEM                                          \
	"-----BEGIN CERTIFICATE----- \r\n"                                      \
	"[ROOT CA CRT PEM]                                       \r\n"  \
	"-----END CERTIFICATE----- "
	const char amazonIoTRootCaPem[] = AMAZON_IOT_ROOT_CA_PEM;

	#define CELINT_KEY_CRT_PEM                                              \
	"-----BEGIN CERTIFICATE----- \r\n"                                      \
	"[YOUR CLIENT KEY CRT PEM]                                       \r\n"  \
	"-----END CERTIFICATE----- "
	const char clientKeyCrtPem[] = CELINT_KEY_CRT_PEM;

	#define CELINT_KEY_PEM                                                  \
	"-----BEGIN RSA PRIVATE KEY-----\r\n"                                   \
	"[YOUR PRIVATE KEY PEM]                                          \r\n"  \
	"-----END RSA PRIVATE KEY----- "
	const char clientKeyPem[] = CELINT_KEY_PEM;

	/**
	 * MQTT client("xxxxxxxxxx.amazonaws.com", 8883, callback);
	 **/
	MQTT client("[TLS mqtt server]", 8883, callback);

	// recieve message
	void callback(char* topic, byte* payload, unsigned int length) {
	    char p[length + 1];
	    memcpy(p, payload, length);
	    p[length] = NULL;
	    String message(p);
	}


	void setup() {
	    // enable tls. set Root CA pem, private key file.
	    client.enableTls(amazonIoTRootCaPem, sizeof(amazonIoTRootCaPem),
	                     clientKeyCrtPem, sizeof(clientKeyCrtPem),
	                     clientKeyPem, sizeof(clientKeyPem));
					 
    Serial.println("tls enable");

    // connect to the server
    client.connect("sparkclient");

    // publish/subscribe
    if (client.isConnected()) {
        Serial.println("client connected");
        client.publish("myfirstthing/outtest", "hello world!");
        client.subscribe("myfirstthing/intest");
    }
	}

	void loop() {
	    if (client.isConnected()) {
	        client.loop();
	    }
	    delay(200);
	}

The issue I personally faced because I was using an older version of the library had to do with keep-alive timeouts. This has now been incorporated into the library via a new constructor format: MQTT client("iot.eclipse.org", 1883, **60**, callback);

Head over to the AWS IoT test client and subscribe to the 'myfirstthing/outtest' topic. You can also publish to the 'myfirstthing/intest' topic to checkout how the Photon behaves when it subscribed to topics.

You can also do the above using MQTT fx if you are more comfortable with that.



# Publishing Sensor Data via MQTT

Amazon expects you to do more with whatever data you publish to the IoT core service and has provided a number of services associated with this. For this project, we chose to write the sensor data to a DynamoDB table. 

Although the services are readily available, there are some rules to follow and one of such rules is the format of the message you publish to the service. You will recall from the inbuilt client's test above that there was a message in green which read **'We cannot display the message as JSON, and are instead displaying it as UTF-8 String.'**

Although generic strings works fine, Amazon expects you to provide your data in JSON format so the sensor in the next sketch, we will explore how to do this using the sensor data.

    #include "application.h"
	#include "math.h"
	#include "src_mqtt/MQTT-TLS.h"
	#include "src/quaternionFilters.h"
	#include "src/BME280.h"
	#include "src/constants.h"
	#include "cert/certs.h"


	//********************************//
	//SENSOR OBJECTS
	//********************************//
	BME280 myBME;


	//********************************//
	//MQTT/TLS VARIABLES
	//********************************//
	void callback(char* topic, byte* payload, unsigned int length);
	const char amazonIoTRootCaPem[] = AMAZON_IOT_ROOT_CA_PEM;
	const char clientKeyCrtPem[] = CELINT_KEY_CRT_PEM;
	const char clientKeyPem[] = CELINT_KEY_PEM;

	unsigned long lastLogTime = millis();


	MQTT client("a6pvdatjymurb.iot.us-west-2.amazonaws.com", 8883, callback);

	// recieve message
	void callback(char* topic, byte* payload, unsigned int length)
	{
	    char p[length + 1];
	    memcpy(p, payload, length);
	    p[length] = NULL;
	    String message(p);
	    if (DBGLVL2) Serial.println(message);
	}

	String constructMQTTPayload(double temp, double pres, double hum, double ax, double ay, double az)
	{
	  String ret = "{";
	  ret += "\"device\":";
	  ret += "\"iot_prj02_sensor_module\",";
	  ret += "\"temperature\":";
	  ret += "\""+String(temp)+"\",";
	  ret += "\"pressure\":";
	  ret += "\""+String(pres)+"\",";
	  ret += "\"humidity\":";
	  ret += "\""+String(hum)+"\",";
	  ret += "}";

	  if (DBGLVL2) Serial.println(ret);

	  return ret;
	}

	void logSensorData(String msg)
	{
	  lastLogTime = millis();
	  if (DBGLVL2) Serial.println(msg);
	  client.publish(MQTT_TOPIC, msg);
	}

	//********************************//
	//PARTICLE SETUP
	//********************************//
	void setup()
	{
	    // Initialise I2C communication as MASTER
	    Wire.begin();
	    // Initialise Serial communication, set baud rate = 9600
	    Serial.begin(9600);
	    delay(300);


	     // enable tls. set Root CA pem, private key file.
	     client.enableTls(amazonIoTRootCaPem, sizeof(amazonIoTRootCaPem),
	                      clientKeyCrtPem, sizeof(clientKeyCrtPem),
	                      clientKeyPem, sizeof(clientKeyPem));
	     if (DBGLVL0) Serial.println("tls enable");

	     // connect to the server
	     client.connect("sparkclient");

	     // publish/subscribe
	     if (client.isConnected()) {
	         if (DBGLVL0) Serial.println("client connected");
	         client.publish(MQTT_TOPIC, "I'm alive");
	         //client.subscribe("iot_prj02_sensor");
	     }
	}

	//********************************//
	//PARTICLE LOOP
	//********************************//
	void loop()
	{
	    if (client.isConnected()) {
	        client.loop();
	    }

	    if(millis() - lastLogTime > 10000)
	    {
	        String msg = constructMQTTPayload(myBME.bme280_getFtemp(), myBME.bme280_getPressure(), myBME.bme280_getHumidity(), myIMU.ax, myIMU.ay, myIMU.az);
	        logSensorData(msg);
	    }


	}



The whole point of this sketch is to format the sensor data appropriately before publishing it with the MQTT broker (Iot Core). For reference purposes, the entire published string for this application looks like this:

    {
	  "device": "iot_prj02_sensor_module",
	  "temperature": ""79.523398",
	  "pressure": "1041.213648",
	  "humidity": "0.000000",
	}

When data of this format is published, the error I mentioned in the text above does not show up and this generally means, the AWS service can do more with the data.
