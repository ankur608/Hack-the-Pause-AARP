#include <Arduino_HTS221.h>
#include <ArduinoBLE.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
MAX30105 particleSensor;
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
float T;

BLEService sensorService("180D");  // Environmental_Sensor-Service
BLEShortCharacteristic sensorLevelChar1("2A6E", BLERead | BLENotify);   // Temperature-Characteristic
BLEShortCharacteristic sensorLevelChar2("2A37", BLERead | BLENotify);   // HR-Characteristic
BLEShortCharacteristic sensorLevelChar3("2A98", BLERead | BLENotify);    // P_Weight-Characteristic

int oldSensorLevel1 = 0; 
int oldSensorLevel2 = 0;
int oldSensorLevel3 = 0;
long previousMillis = 0; 

void setup() {
  Serial.begin(9600);
  while (!Serial); 
  if (!HTS.begin()) {
    while (1);
  }
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  if (!BLE.begin()) {
    Serial.println("Bluetooth init. failed!");
    while (1);
  }
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }  
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);

  BLE.setLocalName("B&ND");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(sensorLevelChar1);
  BLE.addService(sensorService);
  sensorLevelChar1.writeValue(oldSensorLevel1);
  
  sensorService.addCharacteristic(sensorLevelChar2);
  BLE.addService(sensorService);
  sensorLevelChar2.writeValue(oldSensorLevel2);

  sensorService.addCharacteristic(sensorLevelChar3);
  BLE.addService(sensorService);
  sensorLevelChar3.writeValue(oldSensorLevel3);

  // start advertising
  BLE.advertise();
  Serial.println("Bluetooth active, waiting to be connected.....");
}

void loop() {
  // waiting for a BLE central device.
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    digitalWrite(22, HIGH);  // indicate connection
    digitalWrite(24, HIGH);
    while (central.connected()) {
      long currentMillis = millis();
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        updateSensorLevel1();
        updateSensorLevel2();
        updateSensorLevel3();
      }
         long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  if (irValue < 50000)
  Serial.print(" B&ND removed!");
  Serial.println();
    }
    digitalWrite(22, LOW);
    digitalWrite(24, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void updateSensorLevel1() {
  float temperature = HTS.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(temperature, 0);
  T = temperature;
  Serial.println(" °C");
  float sensorLevel1 = map(temperature, 0, 100, 0, 10000);
  sensorLevelChar1.writeValue(sensorLevel1);
  oldSensorLevel1 = sensorLevel1;
}

void updateSensorLevel2() {
  float HR = beatAvg;
  float sensorLevel2 = map(HR, 0, 100, 0, 10000);
  sensorLevelChar2.writeValue(sensorLevel2); 
  oldSensorLevel2 = sensorLevel2; 
}

void updateSensorLevel3() {
  float weight = 74;  
  Serial.print("Weight = ");
  Serial.print(weight);
  Serial.println(" in Kgs.");
  float sensorLevel3 = map(weight, 0, 100, 0, 10000);
  sensorLevelChar3.writeValue(sensorLevel3); 
  oldSensorLevel3 = sensorLevel3; 
}
