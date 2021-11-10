#include "stdlib.h"
#include <Arduino_APDS9960.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);   
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor!");
  } 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (APDS.proximityAvailable()) {
    // read the proximity
    // - 0   => close
    // - 255 => far
    // - -1  => error
    int proximity = APDS.readProximity();

    // print value to the Serial Monitor
    Serial.println(proximity);
  }
}

char* prints(char* out)
{
  int  score = 123;
  char scoreCh[10];
  char str1[] = "Command recognized:";
  char str2[] = " at ";
  char str3[] = " % confidence";
  strcat(out, str1);
  strcat(out, str2);
  strcat(out, itoa(score, scoreCh, 10));
  strcat(out, str3);
}
