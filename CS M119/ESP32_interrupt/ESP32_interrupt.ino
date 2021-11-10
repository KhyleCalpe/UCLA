// ESP32_interrupt.ino

#define inpPin 27
#define outLED 33

unsigned long currTime, prevTime, interruptTime, startTime, onTime, cycleTime;
unsigned int  duty;
float         T=2000, D=0.25, period;
bool          isStart, isOn, isOff;

void IRAM_ATTR isr() 
{
  // Check if a cycle has started
  if (!isStart && digitalRead(inpPin))
  {
    startTime = millis();
    isStart   = true;
    isOn      = true;
    return;
  }
  // Check for duty cycle
  if (isOn && !digitalRead(inpPin))
  {
    onTime = millis() - startTime;
    isOff  = true;
    return;
  }
  // Check for period
  if (isOff && digitalRead(inpPin))
  {
    cycleTime = millis() - startTime;
    isStart   = false;
    isOn      = false;
    isOff     = false;
    // Calculate period and duty cycle
    period = cycleTime / 1000.f;
    duty   = 100.f * onTime / cycleTime;
    // Print period and duty cycle
    Serial.print("Period: ");
    Serial.print(period);
    Serial.print(" s, Duty Cycle: ");
    Serial.print(duty);
    Serial.println(" %");  
  }
}

void setup() 
{
  // Begin Serial Communication
  Serial.begin(9600);
  // Pin Initializations
  pinMode(inpPin, INPUT);
  pinMode(outLED, OUTPUT);
  // Interrupt Initialization
  attachInterrupt(inpPin, isr, CHANGE);  
}

void loop() 
{
  currTime = millis();
  if(currTime-prevTime<=T*D) 
  {
    digitalWrite(outLED, HIGH);
  } 
  else 
  {
    digitalWrite(outLED, LOW);
  }
  if(currTime-prevTime>=T) prevTime = currTime;
}
