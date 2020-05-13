#define TIME 2000
#include <Arduino_HTS221.h>
#include <Arduino_LSM9DS1.h>

double absolute(double x) {
  return x < 0 ? -x : x;
}

void setup() {
  Serial.begin(9600);
    while (!Serial);
  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }
    if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void getHumidTemp() {
  float temperature = HTS.readTemperature();
  float humidity    = HTS.readHumidity();
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity    = ");
  Serial.print(humidity);
  Serial.println(" %");
}

void getLight() {
  int lightValue = 5;
}

double max_activity = 0;

void calcMotion() {
  float x, y, z, i, j ,k;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    //Serial.print(x);
    //Serial.print('\t');
    //Serial.print(y);
    //Serial.print('\t');
    //Serial.println(z);
  }
    if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(i, j ,k);

    //Serial.print(i);
    //Serial.print('\t');
    //Serial.print(j);
    //Serial.print('\t');
    //Serial.println(k);
  }
  
  if (x > 1) x = 2;
  else if (x < 1) x = 1;
  if (k > 210) k = 210;
  else if (k < -250) k = -250;
  if (j > 60) j = 60;
  else if (j < -70) j = -70;
  double activity = ((double(absolute(x - 1)) / 2) + (double(absolute(k + 130)) / 70) + (double(absolute(j)) / 70)) / 3;
  if (activity > max_activity) max_activity = activity;
}

void getMotion() {
  Serial.print("Activity:");
  Serial.println(max_activity);
}
int ms = 0;

void loop() {
  if (ms == 100) {
    getHumidTemp();
    getLight();
    getMotion();
    Serial.println("break");
    max_activity = 0;
    ms = 0;
  }
  calcMotion();
  ms++;
  delay(10);
}
