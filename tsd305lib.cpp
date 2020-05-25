/*
* Description :   Source file of TSD305 Sensor (by TEConnectivity) for Arduino platform.
* Author      :   Pranjal Joshi
* Date        :   25/05/2020 
* License     :   MIT
* This code is published as open source software. Feel free to share/modify.
*/

#include "tsd305lib.h"
#include <Wire.h>

#if (defined(AVR))
	#include <avr\pgmspace.h>
#else
	#include <pgmspace.h>
#endif

#define TSD_ADDR    (uint8_t)0x1E
#define TSD_ADC_CMD (uint8_t)0xAF
#define CONV_DLY    100

bool DEBUG = false;

tsd305::tsd305(void) {
	// Constructor
}

tsd_eeprom_struct tsd305::begin(bool debug) {
  Wire.begin();
  if(debug) {
  	DEBUG = true;
    Serial.println();
  }
  return tsdReadEeprom();
}

bool tsd305::isConnected() {
  Wire.beginTransmission(TSD_ADDR);
  if(Wire.endTransmission() == 0) {
    if(DEBUG)
      Serial.println("TSD305 Connected on I2C bus.");
    return true;
  }
  if(DEBUG)
    Serial.println("TSD305 Not Connected.");
  return false;
}

// Read EEPROM addr - TYPECASTING req for int16_t
uint16_t tsd305::tsdReadCoef(uint8_t addr) {
  uint8_t buf[3];
  Wire.beginTransmission(TSD_ADDR);
  Wire.write(addr);
  Wire.endTransmission();
  delay(1);
  Wire.requestFrom(TSD_ADDR, 3U);
  for(byte i=0;i<3;i++) {
    buf[i] = Wire.read();
  }
  return ((buf[1] << 8) | buf[2]);
}

// Get float value from TSD EEPROM
float tsd305::tsdReadFloat(uint8_t addr) {
  uint16_t hw,lw;
  uint32_t w;
  hw = tsdReadCoef(addr);
  lw = tsdReadCoef(addr+1);
  w = (uint32_t)(hw << 16 | lw);
  float y = *(float*)&w;
  return y;
}

tsd_eeprom_struct tsd305::tsdReadEeprom() {
  tsd_eeprom_struct s;
  s.amb_min = (int16_t)tsdReadCoef(0x1A);
  s.amb_max = (int16_t)tsdReadCoef(0x1B);
  s.obj_min = (int16_t)tsdReadCoef(0x1C);
  s.obj_max = (int16_t)tsdReadCoef(0x1D);
  s.adc_cal = (int16_t)tsdReadCoef(0x2C);
  if(DEBUG) {
    Serial.println("Reading TSD305 EEPROM Parameters:");
    Serial.print("[+] amb_min: ");
    Serial.println(s.amb_min,DEC);
    Serial.print("[+] amb_max: ");
    Serial.println(s.amb_max,DEC);
    Serial.print("[+] obj_min: ");
    Serial.println(s.obj_min,DEC);
    Serial.print("[+] obj_max: ");
    Serial.println(s.obj_max,DEC);
    Serial.print("[+] adc_cal: ");
    Serial.println(s.adc_cal,DEC);
  }
  return s;
}

void tsd305::tsdReadADCs(uint32_t *amb, uint32_t *obj) {
  uint8_t buf[7], i;
  Wire.beginTransmission(TSD_ADDR);
  Wire.write(TSD_ADC_CMD);
  if(Wire.endTransmission() != 0) {
    if(DEBUG)
      Serial.println("Failed to start TSD305 ADC conversion.");
  }
  delay(CONV_DLY);
  Wire.requestFrom(TSD_ADDR, 7U);
  for(i=0;i<7;i++)
    buf[i] = Wire.read();

  *amb = ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
  *obj = ((uint32_t)buf[4] << 16) | ((uint32_t)buf[5] << 8) | (uint32_t)buf[6];

  if(DEBUG) {
    Serial.print("Ambient ADC Value (24-Bit): ");
    Serial.println(*amb);
    Serial.print("Object ADC Value (24-Bit): ");
    Serial.println(*obj);
  }
}

float tsd305::getSensorTemp() {
	uint32_t adc_amb, adc_obj;
	tsd_eeprom_struct t;
	tsdReadADCs(&adc_amb, &adc_obj);
	t = tsdReadEeprom();
  float ts = ((adc_amb/(pow(2,24)))* (t.amb_max - t.amb_min) + t.amb_min);
  if(DEBUG) {
    Serial.print("Sensor Temp (°C): ");
    Serial.println(ts);
  }
  return (float)ts;
}

float tsd305::getTCF(void) {
	float tsens = getSensorTemp();
  float tc, tref, tcf;
  tc = tsdReadFloat(0x1E);
  tref = tsdReadFloat(0x20);
  tcf = 1 + ((tsens-tref)*tc);
  #if DEBUG
    Serial.print("TC Correction Factor: ");
    Serial.println(tcf);
  #endif
  return tcf;
}

void tsd305::tsdGetTempCompensation(float *offset, float *offsettc) {
	float tsens = getSensorTemp();
  float k4,k3,k2,k1,k0;
  k4 = tsdReadFloat(0x22);
  k3 = tsdReadFloat(0x24);
  k2 = tsdReadFloat(0x26);
  k1 = tsdReadFloat(0x28);
  k0 = tsdReadFloat(0x2A);
  *offset = (k4*pow(tsens,4))
            + (k3*pow(tsens,3))
            + (k2*pow(tsens,2))
            + (k1*tsens)
            + k0;
  *offsettc = *offset * getTCF();
  if(DEBUG) {
    Serial.println("Temperature Compensation:");
    Serial.print("[+] Offset: ");
    Serial.println(*offset);
    Serial.print("[+] Offset-TC: ");
    Serial.println(*offsettc);
  }
}

float tsd305::getObjectTemp() {
  float tk4,tk3,tk2,tk1,tk0;
  float adc_comp, adc_comptc;
  float tempOut;

  float tsens = getSensorTemp();

  float offset, offsettc;
  tsdGetTempCompensation(&offset, &offsettc);

  uint32_t adc_obj, adc_amb;
  tsdReadADCs(&adc_amb, &adc_obj);

  tk4 = tsdReadFloat(0x2E);
  tk3 = tsdReadFloat(0x30);
  tk2 = tsdReadFloat(0x32);
  tk1 = tsdReadFloat(0x34);
  tk0 = tsdReadFloat(0x36);
  adc_comp = offsettc + adc_obj - pow(2,23);
  adc_comptc = adc_comp / getTCF();
  tempOut = tk4*pow(adc_comptc,4)
            + tk3*pow(adc_comptc,3)
            + tk2*pow(adc_comptc,2)
            + tk1*adc_comptc
            + tk0;
  if(DEBUG) {
    Serial.print("Object Temperature (°C): ");
    Serial.println(tempOut);
  }
  return tempOut;
}

float tsd305::DtoF(float temp) {
  return (temp*9/5)+32.0;
}
