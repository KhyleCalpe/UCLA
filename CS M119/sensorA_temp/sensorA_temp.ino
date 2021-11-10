// sensorA_temp.ino

#include <Arduino_HTS221.h>

void setup() {
  // Begin serial communication
  Serial.begin(9600);
  while(!Serial);
  // Initialize HTS 
  if(!HTS.begin())
  {
    Serial.println("Failed to initialize humidity/temperature sensor!");
    while(1);    
  }
}

void loop() {
  // Print temperature reading
  Serial.print("Temperature = ");
  Serial.print(HTS.readTemperature());
  Serial.println(" °C");
  // Wait 1 second before logging a reading
  delay(100);
}
