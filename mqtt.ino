#include "application.h"
#include "math.h"
#include "src_mqtt/MQTT-TLS.h"
#include "src/BME280.h"
#include "src/constants.h"


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
