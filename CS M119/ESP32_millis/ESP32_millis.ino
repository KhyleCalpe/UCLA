// ESP32_millis.ino

#define LED 33

const long delayTime = 100;
unsigned long prevTime = 0;

float T = 2000;
float D = 0.25;  

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  unsigned long currTime = millis();
  if(currTime-prevTime<=T*D) 
  {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW); 
  }
  if(currTime-prevTime>=T) prevTime = currTime;
}
