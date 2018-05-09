#include "BME280.h"

void BME280::bme280_Start_I2C_Transmission_1()
{
  for(int i = 0; i < 24; i++)
  {
      // Start I2C Transmission
      Wire.beginTransmission(Addr);
      // Select data register
      Wire.write((136+i));
      // Stop I2C Transmission
      Wire.endTransmission();

      // Request 1 byte of data
      Wire.requestFrom(Addr, 1);

      // Read 24 bytes of data
      if(Wire.available() == 1)
      {
        b1[i] = Wire.read();
      }
  }
}


void BME280::bme280_Start_I2C_Transmission_2()
{
  for(int i = 0; i < 7; i++)
  {
      // Start I2C Transmission
      Wire.beginTransmission(Addr);
      // Select data register
      Wire.write((225+i));
      // Stop I2C Transmission
      Wire.endTransmission();

      // Request 1 byte of data
      Wire.requestFrom(Addr, 1);

      // Read 7 bytes of data
      if(Wire.available() == 1)
      {
        b1[i] = Wire.read();
      }
  }
}


void BME280::bme280_Start_I2C_Transmission_3()
{
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(161);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Request 1 byte of data
  Wire.requestFrom(Addr, 1);

  // Read 1 byte of data
  if(Wire.available() == 1)
  {
    dig_H1 = Wire.read();
  }

}



void BME280::bme280_Start_I2C_Transmission_4()
{
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select control humidity register
  Wire.write(0xF2);
  // Humidity over sampling rate = 1
  Wire.write(0x01);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select control measurement register
  Wire.write(0xF4);
  // Normal mode, temp and pressure over sampling rate = 1
  Wire.write(0x27);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select config register
  Wire.write(0xF5);
  // Stand_by time = 1000ms
  Wire.write(0xA0);
  // Stop I2C Transmission
  Wire.endTransmission();

  for(int i = 0; i < 8; i++)
  {
      // Start I2C Transmission
      Wire.beginTransmission(Addr);
      // Select data register
      Wire.write((247+i));
      // Stop I2C Transmission
      Wire.endTransmission();

      // Request 1 byte of data
      Wire.requestFrom(Addr, 1);

      // Read 8 bytes of data
      if(Wire.available() == 1)
      {
          data[i] = Wire.read();
      }
  }

}


void BME280::bme280_Initialize_temp_coefficents()
{
  // Convert the data
  // temp coefficents
  dig_T1 = (b1[0] & 0xff) + ((b1[1] & 0xff) * 256);
  dig_T2 = b1[2] + (b1[3] * 256);
  dig_T3 = b1[4] + (b1[5] * 256);

  // pressure coefficents
  dig_P1 = (b1[6] & 0xff) + ((b1[7] & 0xff ) * 256);
  dig_P2 = b1[8] + (b1[9] * 256);
  dig_P3 = b1[10] + (b1[11] * 256);
  dig_P4 = b1[12] + (b1[13] * 256);
  dig_P5 = b1[14] + (b1[15] * 256);
  dig_P6 = b1[16] + (b1[17] * 256);
  dig_P7 = b1[18] + (b1[19] * 256);
  dig_P8 = b1[20] + (b1[21] * 256);
  dig_P9 = b1[22] + (b1[23] * 256);

}


void BME280::bme280_humidity_coefficents()
{
  // Convert the data
  // humidity coefficents
  dig_H2 = b1[0] + (b1[1] * 256);
  dig_H3 = b1[2] & 0xFF ;
  dig_H4 = (b1[3] * 16) + (b1[4] & 0xF);
  dig_H5 = (b1[4] / 16) + (b1[5] * 16);
  dig_H6 = b1[6];
}

double BME280::bme280_getCtemp()
{
  // Convert pressure and temperature data to 19-bits
  adc_p = (((long)(data[0] & 0xFF) * 65536) + ((long)(data[1] & 0xFF) * 256) + (long)(data[2] & 0xF0)) / 16;
  adc_t = (((long)(data[3] & 0xFF) * 65536) + ((long)(data[4] & 0xFF) * 256) + (long)(data[5] & 0xF0)) / 16;
  // Convert the humidity data
  adc_h = ((long)(data[6] & 0xFF) * 256 + (long)(data[7] & 0xFF));

  // Temperature offset calculations
  var1 = (((double)adc_t) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
  var2 = ((((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0) *
  (((double)adc_t)/131072.0 - ((double)dig_T1)/8192.0)) * ((double)dig_T3);
  t_fine = (long)(var1 + var2);
  return (var1 + var2) / 5120.0;
}


double BME280::bme280_getFtemp()
{
  // Convert pressure and temperature data to 19-bits
  adc_p = (((long)(data[0] & 0xFF) * 65536) + ((long)(data[1] & 0xFF) * 256) + (long)(data[2] & 0xF0)) / 16;
  adc_t = (((long)(data[3] & 0xFF) * 65536) + ((long)(data[4] & 0xFF) * 256) + (long)(data[5] & 0xF0)) / 16;
  // Convert the humidity data
  adc_h = ((long)(data[6] & 0xFF) * 256 + (long)(data[7] & 0xFF));

  // Temperature offset calculations
  var1 = (((double)adc_t) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
  var2 = ((((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0) *
  (((double)adc_t)/131072.0 - ((double)dig_T1)/8192.0)) * ((double)dig_T3);
  t_fine = (long)(var1 + var2);
  cTemp = (var1 + var2) / 5120.0;
  return cTemp * 1.8 + 32;
}

double BME280::bme280_getPressure()
{
  // Pressure offset calculations
  var1 = ((double)t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
  var2 = var2 + var1 * ((double)dig_P5) * 2.0;
  var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
  var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
  double p = 1048576.0 - (double)adc_p;
  p = (p - (var2 / 4096.0)) * 6250.0 / var1;
  var1 = ((double) dig_P9) * p * p / 2147483648.0;
  var2 = p * ((double) dig_P8) / 32768.0;
  return (p + (var1 + var2 + ((double)dig_P7)) / 16.0) / 100 ;
}


double BME280::bme280_getHumidity()
{
  // Humidity offset calculations
  // Pressure offset calculations
  double t_fine = 0;

  double var_H = (((double)t_fine) - 76800.0);
  var_H = (adc_h - (dig_H4 * 64.0 + dig_H5 / 16384.0 * var_H)) * (dig_H2 / 65536.0 * (1.0 + dig_H6 / 67108864.0 * var_H * (1.0 + dig_H3 / 67108864.0 * var_H)));
  double humidity = var_H * (1.0 -  dig_H1 * var_H / 524288.0);
  if(humidity > 100.0)
  {
      humidity = 100.0;
  }
  else if(humidity < 0.0)
  {
      humidity = 0.0;
  }

  return humidity;
}


void BME280::bme280_init()
{
  bme280_Start_I2C_Transmission_1();

  bme280_Initialize_temp_coefficents();

  bme280_Start_I2C_Transmission_2();

  bme280_humidity_coefficents();

  bme280_Start_I2C_Transmission_3();

  bme280_Start_I2C_Transmission_4();
}
