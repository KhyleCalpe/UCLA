// ESP32_delay.ino

#define LED 33

float T = 2000;
float D = 0.25;  

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(T*D);
  digitalWrite(LED, LOW);
  delay(T-T*D);
}
