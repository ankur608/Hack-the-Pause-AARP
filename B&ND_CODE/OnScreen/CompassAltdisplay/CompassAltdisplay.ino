#define DECLINATION +0.36 // User location-specific.
#include <Arduino_LSM9DS1.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>
#define PIN_RESET 9  
#define DC_JUMPER 1 
MicroOLED oled(PIN_RESET, DC_JUMPER);
#define blinkPin 24
float Po = 1013.25; // average pressure at mean sea-level (MSL) in the International Standard Atmosphere (ISA) is 1013.25 hPa
float P, T;
float To = 273.15;
float k = 0.0065;

void setup() {
  Serial.begin(9600);
  IMU.begin();
  while (!Serial);
  if (!HTS.begin()) {
    while (1);
  }
  if (!BARO.begin()) {
    while (1);
  }
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  Wire.begin();
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  
  delay(1000);     
  oled.clear(PAGE); 
}

void loop() {
  float temperature = HTS.readTemperature();
 // float humidity    = HTS.readHumidity();
  float pressure = BARO.readPressure();
  T = temperature;
  P = pressure*10;  // in hectoPascals. // [1hPa = 10kPa]
  //Serial.print(P);
  //Serial.println(" hPa");
  float fP = (Po/P);
  //Serial.println(fP, 12);  // pressure-factor.
  float x = (1/5.257);
  //Serial.println(x, 12);   // power-constant.
  double z = pow(fP,x);    // exponential pressure-grad.
  //Serial.println(z, 8);
  float height = (((z-1)*(T+To)) / k);
  int alt = height;
  Serial.print("Altitude = ");
  Serial.print(height, 1);
  Serial.println(" metres");
  delay(200);  // ceases under 100ms.
  
  float l, m, n;
  if (IMU.magneticFieldAvailable()) 
  {
    IMU.readMagneticField(l, m, n);
  float yaw;
  if (m == 0)
    yaw = (l < 0) ? PI : 0;
  else
    yaw = atan2(l, m);
    yaw -= DECLINATION * PI / 180;
  if (yaw > PI) yaw += (PI);
  else if (yaw > -PI) yaw += (PI);
  // Converting Radians to Degrees:
  yaw *= 180.0 / PI;
  //Serial.print("  Yaw:  "); 
  //Serial.println(yaw, 0);
  int Heading = yaw;
  Serial.println(Heading);
  
  int middleZ = (oled.getLCDWidth() / 2)-14;
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;
  oled.clear(PAGE);
  oled.setCursor(middleX - (oled.getFontWidth() * (2)-7),
                 middleY - (oled.getFontHeight() / 2)-17);
  if (Heading >= 0 && Heading <= 3)
  {
    Serial.println("NORTH [N]");
    oled.print("N");
    digitalWrite(22, LOW);
    digitalWrite(23, LOW);
  }
  else if (Heading >=357 && Heading <=360)
  {
    Serial.println("NORTH [N]");
    oled.print("N");
    digitalWrite(22, LOW);
    digitalWrite(23, LOW);
  }
  else if (Heading >=267 && Heading <=273)
  {
    Serial.println("EAST [E]");
    oled.print("E");    // +_2
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
  }
  else if (Heading >=177 && Heading <=183)
  {
    Serial.println("SOUTH [S]");
    oled.print("S");
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
  }
  else if (Heading >=87 && Heading <=93)
  {
    Serial.println("WEST [W]");
    oled.print("W");
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
  }
    else if (Heading >=305 && Heading <=325)
  {
    Serial.println("NORTH-EAST [NE]");
    oled.print("NE");    // +_10
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
  }
    else if (Heading >=35 && Heading <=55)
  {
    Serial.println("NORTH-WEST [NW]");
    oled.print("NW");
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
  }
    else if (Heading >=215 && Heading <=235)
  {
    Serial.println("SOUTH-EAST [SE]");
    oled.print("SE");
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
  }
    else if (Heading >=125 && Heading <=145)
  {
    Serial.println("SOUTH-WEST [SW]");
    oled.print("SW");
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
  }
                 
  //oled.print("Direction");
  oled.setCursor(middleX - (oled.getFontWidth() * (2)+5),
                 middleY - (oled.getFontHeight() / 2)-2);
  oled.print(Heading);  
  oled.print("deg");
 // oled.setCursor(middleX - (oled.getFontWidth() * (2)+10),
   oled.setCursor(middleZ - (oled.getFontWidth() * (2)+4),
                 middleY - (oled.getFontHeight() / 2)+11);
  oled.print("__________");
  oled.print("ALT:");
  oled.print(alt);
  oled.print(" m.");
  oled.display();
  oled.clear(PAGE); 
 }
}
