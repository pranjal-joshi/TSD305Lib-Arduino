/*
* Description :   Example of TSD305 Sensor (by TE-Connectivity) for Arduino platform.
* Author      :   Pranjal Joshi
* Date        :   25/05/2020 
* License     :   MIT
* This code is published as open source software. Feel free to share/modify.
*/

#include <tsd305lib.h>

tsd305 tsd;
tsd_eeprom_struct tes;

float sensorTemp, objDeg, objFar;

void setup() {
  bool debug = false;                    // Change to true to read sensor parameters while calling other functions       
  Serial.begin(115200);                 // Change baudrate according to your application
  Serial.println();
  tes = tsd.begin(debug);               // Prints debuging values over serial while accessing TSD305
  /*
   If sensor debugging is not required, you can use following as well
   tes = tsd.begin();
   If sensor's measurement limits are not required, use following
   tsd.begin();
   */
}

void loop() {
  if(tsd.isConnected()) {               // Check if sensor is connected over I2C bus
    
    sensorTemp = tsd.getSensorTemp();   // Get temperature of sensor itself i.e. ambient temperature
    objDeg = tsd.getObjectTemp();       // Get temperature of object
    objFar = tsd.DtoF(objDeg);          // Convert value from °C to °F

    // Print measured data on serial monitor
    Serial.print("Sensor (Ambient) Temperature (°C): ");
    Serial.println(sensorTemp);
    Serial.print("Object Temperature (°C): ");
    Serial.println(objDeg);
    Serial.print("Object Temperature (°F): ");
    Serial.println(objFar);
    Serial.println("\n");
  }
  else {
    Serial.println("TSD305 not connected.");
  }
  delay(1000);
}