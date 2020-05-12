#include <Arduino_HTS221.h>
#include <Wire.h>
#include "MAX30105.h"
#include <AnalogSmooth.h>
int analogPin = A3;
AnalogSmooth as = AnalogSmooth();
AnalogSmooth as100 = AnalogSmooth(100);
MAX30105 particleSensor;

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  if (!HTS.begin()) {
    Serial.println("Failed to initialize ambient sensor!");
    while (1);
  }
  Serial.println("Initializing.Hydra.v1.");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
  byte sampleAverage = 8; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR,
  int sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void loop()
{
  const byte avgAmount = 64;
  long baseValue = 0;
  for (byte x = 0 ; x < avgAmount ; x++)
  {
    baseValue += particleSensor.getIR(); //Read the IR value
  }
  baseValue /= avgAmount;
  for (int x = 0 ; x < 500 ; x++)
  Serial.println(baseValue);
  float k = baseValue;
  Serial.println(particleSensor.getIR()); //Send raw data to plotter
  float analog = analogRead(analogPin);
  //delay(50);
  float analogSmooth100 = as100.analogReadSmooth(analogPin);
  Serial.print("Dermal Response(Hydration Quotient): ");  
  float h2o = analogSmooth100*385; 
  Serial.println(h2o/k);
  float temperature = HTS.readTemperature();
  float humidity    = HTS.readHumidity();
  // float hq > 5;    // Reject..
  if ( (k > h2o+temperature) && k > 74000 && (h2o > 50000+humidity) )
  { Serial.println("Re-hydrate yourself.");
  } 
}
