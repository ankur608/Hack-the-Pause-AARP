#include <Arduino_LSM9DS1.h>
float threshhold=0.8;
float xval[100]={0};
float yval[100]={0};
float zval[100]={0};
float xavg;
float yavg;
float zavg;
int steps,flag=0;
float x,y,z;

void setup() {
pinMode(22,OUTPUT);
calibrate();
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
}

void loop() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
int acc=0;
float totvect[100]={0};
float totave[100]={0};
//float sum1,sum2,sum3=0;
float xaccl[100]={0};
float yaccl[100]={0};
float zaccl[100]={0};
   
for (int i=0;i<20;i++)
{
xaccl[i]=float(x);
delay(1);
yaccl[i]=float(y);
delay(1);
zaccl[i]=float(z);
delay(1);
totvect[i] = sqrt(((xaccl[i]-xavg)* (xaccl[i]-xavg))+ ((yaccl[i] - yavg)*(yaccl[i] - yavg)) + ((zval[i] - zavg)*(zval[i] - zavg)));
totave[i] = (totvect[i] + totvect[i-1]) / 2 ;
Serial.println(totave[i]);
delay(200);

//cal steps 
if (totave[i]>threshhold && flag==0)
{
  steps=steps+1;
  flag=1;
}
 else if (totave[i] > threshhold && flag==1)
{
//do nothing 
}
  if (totave[i] <threshhold  && flag==1)
  {flag=0;}
  Serial.println('\n');
  Serial.print("steps=");
  Serial.println(steps);
}

 delay(1000);
  } 
}

void calibrate()
{
  digitalWrite(22,HIGH); 
  float sum=0;
  float sum1=0;
  float sum2=0;
for (int i=0;i<100;i++)
{
xval[i]=float(x);
sum=xval[i]+sum;
}
delay(100);
xavg=sum/100.0;
Serial.println(xavg);
for (int j=0;j<100;j++)
{
xval[j]=float(x);
sum1=xval[j]+sum1;
}
yavg=sum1/100.0;
Serial.println(yavg);
delay(100);
for (int i=0;i<100;i++)
{
zval[i]=float(z);
sum2=zval[i]+sum2;
}
zavg=sum2/100.0;
delay(100);
Serial.println(zavg);
digitalWrite(22,LOW);
}
