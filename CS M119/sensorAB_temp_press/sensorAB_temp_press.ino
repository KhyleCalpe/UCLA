// sensorAB_temp_press.ino

#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>

const long delayA = 5000;
const long delayB = 1000;
unsigned long prevTimeA = 0;
unsigned long prevTimeB = 0;

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
  // Initialize LPS
  if(!BARO.begin())
  {
    Serial.println("Failed to initialize pressure sensor!");
    while(1);    
  }
}

void loop() {
  // Keep track of time
  unsigned long currTime = millis();
  // Check for sensor A
  if (currTime - prevTimeA >= delayA)
  {
    // Print temperature reading
    Serial.print("Sensor A: Temperature = ");
    Serial.print(HTS.readTemperature());
    Serial.println(" °C");
    prevTimeA = currTime;
  }
  // Check for sensor B
  if (currTime - prevTimeB >= delayB)
  {
    // Print temperature reading
    Serial.print("Sensor B: Pressure = ");
    Serial.print(BARO.readPressure());
    Serial.println(" kPa");
    prevTimeB = currTime; 
  }
}
