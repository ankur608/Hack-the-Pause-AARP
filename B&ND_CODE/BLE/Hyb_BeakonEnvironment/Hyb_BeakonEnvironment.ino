#include <Arduino_HTS221.h>
#include <ArduinoBLE.h>
#include <Arduino_LPS22HB.h>
float Po = 1013.25; // average pressure at mean sea-level (MSL) in the International Standard Atmosphere (ISA) is 1013.25 hPa
float P, T;
float To = 273.15;
float k = 0.0065;

BLEService sensorService("181A");  // Environmental_Sensor-Service
BLEShortCharacteristic sensorLevelChar1("2A6E", BLERead | BLENotify);   // Temperature-Characteristic
BLEShortCharacteristic sensorLevelChar2("2A6F", BLERead | BLENotify);   // Humidity-Characteristic
BLELongCharacteristic sensorLevelChar3("2A6C", BLERead | BLENotify);    // Elevation-Characteristic
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
  if (!BARO.begin()) {
    while (1);
  }
  pinMode(23, OUTPUT);

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
    digitalWrite(23, HIGH);  // indicate connection
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
  float humidity    = HTS.readHumidity();
  Serial.print("Humidity = ");
  Serial.print(humidity, 0);
  Serial.println(" %");
  float sensorLevel2 = map(humidity, 0, 100, 0, 10000);
  sensorLevelChar2.writeValue(sensorLevel2); 
  oldSensorLevel2 = sensorLevel2; 
}

void updateSensorLevel3() {
  float pressure = BARO.readPressure();
  P = pressure*10;  // in hectoPascals. // [1hPa = 10kPa]
  float fP = (Po/P);       // pressure-factor.
  float x = (1/5.257);     // power-constant.
  double z = pow(fP,x);    // exponential pressure-grad.
  float height = (((z-1)*(T+To)) / k);
  
  Serial.print("Altitude = ");
  Serial.print(height);
  Serial.println(" metres");
  float sensorLevel3 = map(height, 0, 100, 0, 10000);
  sensorLevelChar3.writeValue(sensorLevel3); 
  oldSensorLevel3 = sensorLevel3; 
}
