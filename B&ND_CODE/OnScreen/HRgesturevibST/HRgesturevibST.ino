#include <Arduino_APDS9960.h>
#include <Arduino_HTS221.h>
#include <SFE_MicroOLED.h>
#define PIN_RESET 9  
#define DC_JUMPER 1 
MicroOLED oled(PIN_RESET, DC_JUMPER);
#define blinkPin 24
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor!");
  }
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);
  pinMode(blinkPin, OUTPUT);
  pinMode(3, OUTPUT);
  Wire.begin();
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  //oled.display();
  delay(1000);     
  oled.clear(PAGE); 
}

void loop()
{
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; 
      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  } gesture();
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  if(beatAvg > 135)
  {
    vib();
  }
  if (irValue < 50000)
    Serial.print(" B&ND removed!");
  Serial.println();
}

void vib()
{   digitalWrite(3, HIGH);
  delay(30); 
  digitalWrite(3, LOW);
  //delay(70);  
  }
  
void gesture()
{
  if (APDS.gestureAvailable()) {
    // a gesture was detected, read and print to serial monitor
    int gesture = APDS.readGesture();

    switch (gesture) {

      case GESTURE_LEFT:
        Serial.println("Detected LEFT gesture");
        display();
        break;
        
      case GESTURE_RIGHT:
        Serial.println("Detected RIGHT gesture");
        oled.clear(ALL);
        oled.clear(PAGE);
        break;

      default:
        // ignore
        break;
    }
  }
}

void display()
{
  int middleZ = (oled.getLCDWidth() / 2)-14;
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;
  oled.clear(PAGE);
  oled.setCursor(middleZ - (oled.getFontWidth() * (2)+3),
                 middleY - (oled.getFontHeight() / 2)-17);
  oled.print("HEART-RATE");
  
  //oled.setCursor(middleZ - (oled.getFontWidth() * (2)-6),
  //               middleY - (oled.getFontHeight() / 2)-8);
  oled.setCursor(middleX - (oled.getFontWidth() * (2)+1),
                 middleY - (oled.getFontHeight() / 2));
  oled.print(beatAvg);
  oled.print("bpm");
  //delay(500);
  oled.setCursor(middleX - (oled.getFontWidth() * (2)+1),
                 middleY - (oled.getFontHeight() / 2)+17);
  float temp = HTS.readTemperature();
  if(temp > 46)
  {
    vib();
  }
  oled.print(temp, 0); 
  oled.print(" C"); 
  oled.display();
  oled.clear(PAGE);
}
