 //.......................LIBRARIES....................................//
#include <PID_v1.h>         // PID´s library
#include <stdio.h>          // C++ library for snprintf
#include <string.h>
#include "Wire.h"
//......................DEFINITIONS...................................//
#define InputPin A0 // Level input (0-10V) 
#define OutputPin 3 // Pump´s angular velocity output (0-10V) 
//...................... VARIABLES....................................//

volatile double PV=0;   // Process value 
volatile double setp=30;   // Set point
volatile double ERR=0;  // Error
volatile double CP=0;   // Control process
volatile int Ts=1000;   // Sample time 1s
volatile double Kp=1;   // Proportional constant
volatile double Ti=8.14;   // Integrative constant
volatile double Td=1.02;   // Derivative constant
char bufferparameters[50]; 
char bufferdata[50]; 
volatile float times=0;
volatile char i2c_on_off='1';


//...................... PID ..........................................//

PID myPID(&PV, &CP, &setp, Kp, Ti, Td, DIRECT);

void setup() {
  // PID settings
  myPID.SetMode(AUTOMATIC);
  // Communication settings
  Serial.begin(115200);
  Wire.begin(2);
  Wire.onReceive(comunicationIN);
  Wire.onRequest(comunicationOUT);
}

int now = 0;


void loop() {
  unsigned long StartTime = millis();





 
  
  if (i2c_on_off != '0'){
     PID();
    } else {
    CP=0;
  }  
  

  delay(1000);
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  now = ElapsedTime /1000;
}


void comunicationIN(int bytes){
  int msg_size = 4*sizeof(double) + 1;

  byte* receiver;

  for(i=0; i<msg_size; i++)
    *(receiver + i) = (byte)Wire.read();

  setp=(double) *receiver;
  Serial.print("setp: ");
  Serial.println(setp);
  
  Kp=(double) *(receiver + sizeof(double));
  Serial.print("Kp: ");
  Serial.println(Kp);
  
  Ti=(double) *(receiver + 2*sizeof(double));
  Serial.print("Ti: ");
  Serial.println(Ti);
  
  Td=(double) *(receiver + 3*sizeof(double));
  Serial.print("Td: ");
  Serial.println(Td);
  
  i2c_on_off=(char) *(receiver + 3*sizeof(double) + 1);
  Serial.print("i2c_on_off: ");
  Serial.println(i2c_on_off);
  
}


void comunicationOUT(){ 

   Wire.write((byte*)&PV, sizeof(PV));
   Wire.write((byte*)&CP, sizeof(CP));
   Wire.write((byte*)&now, sizeof(now));
}

void PID() {
 myPID.SetSampleTime(Ts);
 myPID.SetTunings(Kp,Ti,Td);
 double tmp =analogRead(InputPin);
 PV= (tmp*(100/1023.0));
 myPID.Compute();
 analogWrite(OutputPin,CP);
 CP=CP* (100.0/250.0);
}
