 //.......................LIBRARIES....................................//
#include <PID_v1.h>         // PID´s library
#include <stdio.h>          // C++ library for snprintf
#include <string.h>
#include "Wire.h"

//......................DEFINITIONS...................................//
#define InputPin A0 // Level input (0-10V) 
#define OutputPin 3 // Pump´s angular velocity output (0-10V) 

//...................... VARIABLES....................................//
double PV=0;   // Process value 
double setp;   // Set point
double ERR=0;  // Error
double CP;   // Control process
int Ts=1000;   // Sample time 1s
double Kp;   // Proportional constant
double Ti;   // Integrative constant
double Td;   // Derivative constant

char bufferparameters[50]; 
char bufferdata[50]; 

String i2c_on_off;
volatile int now = 0;


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




void loop() {
 
  unsigned long StartTime = millis();     

  Parseinfo();
 
  Serial.println(i2c_on_off);
  if (i2c_on_off == "1" ){
      PID();
    } else {
    CP=0;
  }  
  

  
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  
  now = ElapsedTime /1000;

 if (now < 1){
  now=1;
  Serial.println(now);
  }
  delay(1000);
 
  
}


void Parseinfo(){
  
  char str_PV[8];
  char str_CP[8];
  
  dtostrf(PV,3,2,str_PV);
  dtostrf(CP,3,2,str_CP);
//  Serial.print("String pV: ");
//  Serial.print(str_PV);
//  Serial.print(" PV: ");
//  Serial.print(PV);
//  Serial.print("String CP: ");
//  Serial.print(str_CP);
//  Serial.print(" CP: ");
//  Serial.print(CP);
  sprintf(bufferdata,"%s;%s;%d",str_PV,str_CP,now);
//  Serial.print(" bufferdata : ");
//  Serial.println(bufferdata);
  
  String s_setp,s_kp,s_ti,s_td,s_on_off;
  
  String s_bufferparameters = String(bufferparameters);
  
  char* tempbuffer;
  
  s_setp= strtok(bufferparameters,";");
  s_bufferparameters.remove(0,s_setp.length()+1);
  s_bufferparameters.toCharArray(tempbuffer,s_bufferparameters.length());
  
  s_kp= strtok(tempbuffer,";");
  s_bufferparameters.remove(0,s_kp.length()+1);
  s_bufferparameters.toCharArray(tempbuffer,s_bufferparameters.length());
  
  s_ti= strtok(tempbuffer,";");
  s_bufferparameters.remove(0,s_ti.length()+1);
  s_bufferparameters.toCharArray(tempbuffer,s_bufferparameters.length());
  
  s_td= strtok(tempbuffer,";");
  s_bufferparameters.remove(0,s_td.length()+1);
  s_bufferparameters.toCharArray(tempbuffer,s_bufferparameters.length());
  
  s_on_off= strtok(tempbuffer,";");
  
  setp=s_setp.toDouble();
  Kp=s_kp.toDouble();
  Ti=s_ti.toDouble();
  Td=s_td.toDouble();
  i2c_on_off=s_on_off;
 
}


void comunicationIN(int bytes){
  
  int i=0; 
  
  while (Wire.available()) { 
    bufferparameters[i] = Wire.read(); 
    i=i+1;
  }
}


void comunicationOUT(){ 
  
  Wire.write(bufferdata);
  Serial.println(bufferdata);

}

void PID() {
  
  myPID.SetSampleTime(Ts);
  myPID.SetTunings(Kp,Ti,Td);
//  Serial.print("Parameters KP, Ti, Td :");
//  Serial.print(Kp);
//  Serial.print(",");
//  Serial.print(Ti);
//  Serial.print(",");
//  Serial.println(Td);  
  double tmp =analogRead(InputPin);
  
  PV= (tmp*(100/1023.0));
//  Serial.print("PV and SETP ");
//  Serial.print(PV);
//  Serial.print(" ; ");
//  Serial.print(setp);
  
  myPID.Compute();

  analogWrite(OutputPin,CP);
  
  CP=CP* (100.0/255.0);
}
