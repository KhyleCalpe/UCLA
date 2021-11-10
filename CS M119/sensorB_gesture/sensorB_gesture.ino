// sensorB_gesture.ino

#include <Arduino_APDS9960.h>

void setup() {
  // Begin Serial
  Serial.begin(9600);
  while(!Serial);
  // Initialize APDS
  if(!APDS.begin())
  {
    Serial.println("Failed to initialize proximity/light/colour/gesture sensor!");
    while(1);    
  }
  // Set gesture sensitivity
  APDS.setGestureSensitivity(80);
}

void loop() {
  // Check if a gesture is detected
  if (APDS.gestureAvailable()) 
  {
    // Print the detected gesture
    Serial.print("Gesture detected: ");
    switch(APDS.readGesture())
    {
      case GESTURE_UP:
        Serial.println("UP");
        break;
      case GESTURE_DOWN:
        Serial.println("DOWN");
        break;
      case GESTURE_LEFT:
        Serial.println("LEFT");
        break;
      case GESTURE_RIGHT:      
        Serial.println("RIGHT");
        break;
      default:
        break;
    }
  }
}
