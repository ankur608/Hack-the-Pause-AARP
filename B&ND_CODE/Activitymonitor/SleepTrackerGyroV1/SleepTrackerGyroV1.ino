#include <Arduino_LSM9DS1.h>

long timer = 0;
long sleep_timer_start, sleep_timer_end,sleep_timer_end2;
float x,y,z;
int activate, interrupt, stage_sleep_time, interrupt_sleep_timer, interrupt_for_deep_sleep, total_sleep, total_deep_sleep, total_light_sleep, deep_sleep, light_sleep, interrupt_timer=0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");
  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
}

void loop() {
  float x, y, z;
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);
  }
if(millis() - timer > 1000){ 
int X = x;
int Y = y;
int Z = z;

if (activate == 0){ // first sleep confirmation
if ((X<=20 || X>=-20) && (Y<=20 || Y>=-20) && (Z<=20 || Z>=-20)) {
sleep_timer_start = millis()/1000-sleep_timer_end;
if (sleep_timer_start == 300){
activate = 1;}
}
if ((X>=20 || X<=-20) || (Y>=20 || Y<=-20) || (Z>=20 || Z<=-20)){
sleep_timer_end =(millis()/1000); }
}

if (activate == 1){ // sleeping mode
light_sleep = (millis()/1000)-sleep_timer_end;

if (interrupt == 0){
if (light_sleep >= 4200){
if (interrupt_for_deep_sleep > 4200){
if (light_sleep - interrupt_sleep_timer >= 600){
deep_sleep = light_sleep - interrupt_for_deep_sleep;
}
}
}
}
light_sleep = light_sleep - deep_sleep;

if ((X>=20 || X<=-20) || (Y>=20 || Y<=-20) || (Z>=20 || Z<=-20)){
interrupt_sleep_timer = (millis()/1000)-sleep_timer_end; 
interrupt_for_deep_sleep = light_sleep;
interrupt =interrupt+1;
delay(8000);
}

if ((millis()/1000)- sleep_timer_end -interrupt_sleep_timer > 300) {
interrupt =0; 
}
if ((millis()/1000)- sleep_timer_end - interrupt_sleep_timer <= 300){
if (interrupt >=5){
sleep_timer_end =(millis()/1000);
if (light_sleep >= 900){ // second sleep confirmation
total_light_sleep = total_light_sleep + light_sleep;
total_deep_sleep = total_deep_sleep + deep_sleep;
total_sleep = total_light_sleep + total_deep_sleep; }
activate =0;
interrupt =0;
deep_sleep= 0;
light_sleep= 0;
interrupt_sleep_timer=0;
interrupt_for_deep_sleep=0;
}
}
}

stage_sleep_time = light_sleep + deep_sleep; 
if (stage_sleep_time >= 5400){
sleep_timer_end =(millis()/1000);
total_light_sleep = total_light_sleep + light_sleep;
total_deep_sleep = total_deep_sleep + deep_sleep;
total_sleep = total_light_sleep + total_deep_sleep; 
activate =0;
interrupt =0;
deep_sleep= 0;
light_sleep= 0;
interrupt_sleep_timer=0;
interrupt_for_deep_sleep=0; 
}

Serial.print(sleep_timer_start);
Serial.print(",");
if (light_sleep >= 900){ 
Serial.print(light_sleep/60);
Serial.print(",");
Serial.print(deep_sleep/60);
Serial.print(","); 
Serial.print(total_light_sleep/60);
Serial.print(",");
Serial.print(total_deep_sleep/60);
Serial.print(",");
Serial.print(total_sleep/60);
Serial.println(";"); }

else {
Serial.print(0);
Serial.print(",");
Serial.print(0);
Serial.print(","); 
Serial.print(total_light_sleep/60);
Serial.print(",");
Serial.print(total_deep_sleep/60);
Serial.print(",");
Serial.print(total_sleep/60);
Serial.println(";");
} 
timer = millis();
}
}
