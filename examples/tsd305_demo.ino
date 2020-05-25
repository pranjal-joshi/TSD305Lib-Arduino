
#include <tsd305lib.h>

tsd305 tsd;
tsd_eeprom_struct tes;

float sensorTemp, objDeg, objFar;

void setup() {
  bool debug = false;                   // Change to true to read sensor parameters while calling other functions       
  Serial.begin(115200);
  Serial.println();
  tes = tsd.begin(debug);               // Prints debuging values over serial while accessing TSD305
}

void loop() {
  if(tsd.isConnected()) {
    sensorTemp = tsd.getSensorTemp();
    objDeg = tsd.getObjectTemp();
    objFar = tsd.DtoF(objDeg);
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
