#include <AnalogSmooth.h>
int analogPin = A3;
AnalogSmooth as = AnalogSmooth();
AnalogSmooth as100 = AnalogSmooth(100);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Regular reading
  float analog = analogRead(analogPin);
  delay(50);
  float analogSmooth100 = as100.analogReadSmooth(analogPin);
  Serial.print("Emotional State: ");  
  Serial.println(analogSmooth100);
  int e = analogSmooth100; 
  if ( e < 24 )
  { 
   Serial.println("EDR is not recorded!");
  }
  else if ( e > 25 && e < 180)
  {
   Serial.println("Emotional State: Lighten up a little.");   
  }
  else if ( e > 180 && e < 310)
  {
   Serial.println("Emotional State: Normal");   
  }
    else if ( e > 310 && e < 480)
  {
   Serial.println("Emotional State: Highly Likely");   
  }
  
  //Serial.println(e);
  //delay(1000);
}
