/*
* Description :   Header file of TSD305 Sensor (by TEConnectivity) for Arduino platform.
* Author      :   Pranjal Joshi
* Date        :   25/05/2020 
* License     :   MIT
* This code is published as open source software. Feel free to share/modify.
*/

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

struct tsd_eeprom_struct {
  int16_t amb_min;
  int16_t amb_max;
  int16_t obj_min;
  int16_t obj_max;
  int16_t adc_cal;
};

class tsd305 {

public:
	tsd305();
	void begin(void);
	tsd_eeprom_struct begin(void);
	tsd_eeprom_struct begin(bool debug);
	bool isConnected(void);
	float getSensorTemp(void);
	float getObjectTemp(void);
	float DtoF(float temp);

private:
	uint16_t tsdReadCoef(uint8_t addr);
	float tsdReadFloat(uint8_t addr);
	tsd_eeprom_struct tsdReadEeprom(void);
	void tsdReadADCs(uint32_t *amb, uint32_t *obj);
	float getTCF(void);
	void tsdGetTempCompensation(float *offset, float *offsettc);
};