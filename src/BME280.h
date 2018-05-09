#ifndef _BME280_H_
#define _BME280_H_

#include "application.h"
#include "math.h"

//********************************//
//BME 280 Variables
//********************************//
#define Addr 0x76

class BME280
{
  public:
    unsigned int b1[24];
    unsigned int data[8];

    int dig_H1 = 0;
    int dig_H2 = 0;
    int dig_H3 = 0;
    int dig_H4 = 0;
    int dig_H5 = 0;
    int dig_H6 = 0;

    int dig_T1 = 0;
    int dig_T2 = 0;
    int dig_T3 = 0;

    int dig_P1 = 0;
    int dig_P2 = 0;
    int dig_P3 = 0;
    int dig_P4 = 0;
    int dig_P5 = 0;
    int dig_P6 = 0;
    int dig_P7 = 0;
    int dig_P8 = 0;
    int dig_P9 = 0;

    double cTemp = 0, fTemp = 0, pressure = 0, humidity = 0;

    long adc_p = 0;
    long adc_t = 0;
    long adc_h = 0;

    double var1 = 0;
    double var2 = 0;
    double t_fine = 0;

  public:
    void bme280_Start_I2C_Transmission_1();
    void bme280_Start_I2C_Transmission_2();
    void bme280_Start_I2C_Transmission_3();
    void bme280_Start_I2C_Transmission_4();
    void bme280_Initialize_temp_coefficents();
    void bme280_humidity_coefficents();
    double bme280_getCtemp();
    double bme280_getFtemp();
    double bme280_getPressure();
    double bme280_getHumidity();
    void bme280_init();

};

#endif // _BME280_H_
