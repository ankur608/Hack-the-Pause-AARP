#include <Arduino_HTS221.h>
#include <ArduinoBLE.h>
#include <Arduino_LPS22HB.h>
float T;

BLEService sensorService("181B");  // BodyComp.-Service
BLEShortCharacteristic sensorLevelChar1("2A6E", BLERead | BLENotify);   // Temperature-Characteristic
BLEShortCharacteristic sensorLevelChar2("2A8E", BLERead | BLENotify);   // Height-Characteristic
BLEShortCharacteristic sensorLevelChar3("2A98", BLERead | BLENotify);    // Weight-Characteristic

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
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);
  if (!BLE.begin()) {
    Serial.println("Bluetooth init. failed!");
    while (1);
  }

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
    digitalWrite(23, HIGH);  
    digitalWrite(24, HIGH); // indicate connection
    while (central.connected()) {
      long currentMillis = millis();
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        updateSensorLevel1();
        updateSensorLevel2();
        updateSensorLevel3();
      }
    }
    digitalWrite(23, LOW);
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
  float height = 170; // Subject-height
  Serial.print("Height = ");
  Serial.print(height);
  Serial.println(" in cms.");
  float sensorLevel2 = map(height, 0, 100, 0, 100);
  sensorLevelChar2.writeValue(sensorLevel2); 
  oldSensorLevel2 = sensorLevel2; 
}

void updateSensorLevel3() {
  float weight = 148;  // Subject-weight
  Serial.print("Weight = ");
  Serial.print(weight);
  Serial.println(" in lbs.");
  float sensorLevel3 = map(weight, 0, 100, 0, 10000);
  sensorLevelChar3.writeValue(sensorLevel3); 
  oldSensorLevel3 = sensorLevel3; 
}
