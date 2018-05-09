# Using Particle's Photon as a MQTT client

[MQTT](http://mqtt.org/) is a machine-to-machine (M2M)/"Internet of Things" connectivity protocol. It was designed as an extremely lightweight publish/subscribe messaging transport. It is designed for constrained devices and low-bandwidth, high-latency or unreliable networks.

Particle's [Photon](https://www.particle.io/products/hardware/photon-wifi/) is small and powerful wifi-connected micontroller capable of basic to advaced telemetry when interfaced with sensor units. To demonstrate the use of Photon as an MQTT client, the microcontroller was interfaced with the Bosch Sensortec Intergrated Environmental Sensor Unit _BME280_. To make the unit readily accessible, a breakout version from [Adafruit](https://www.adafruit.com/product/2652) was obtained. 

The scope of this project comprised setting up the microcontroller as a MQTT client, to report telemetry data from the Environmental Sensor Unit to the [Amazon AWS IoT Core](https://aws.amazon.com/iot-core/) service. AWS IoT Core is a managed cloud platform that lets connected devices easily and securely interact with cloud applications and other devices via MQTT/TLS. Once the data reached the service, the secondary objective was to write the data to the [DynamoDB](https://aws.amazon.com/dynamodb/) time-series database which is another AWS service.
