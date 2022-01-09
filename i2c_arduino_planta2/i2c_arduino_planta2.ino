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
double setp=30;   // Set point
double ERR=0;  // Error
double CP=0;   // Control process
int Ts=1000;   // Sample time 1s
double Kp=1;   // Proportional constant
double Ti=8.14;   // Integrative constant
double Td=1.02;   // Derivative constant
char bufferparameters[50]; 
char bufferdata[50]; 
String i2c_on_off;


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

volatile int now = 0;


void loop() {
  unsigned long StartTime = millis();

  Parseinfo();
 




 
  Serial.println(i2c_on_off);
  if (i2c_on_off == "1" ){
      PID();
    } else {
    CP=0;
  }  
  

  delay(1000);
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  now = ElapsedTime /1000;
 
  
}
void Parseinfo(){
 char str_PV[6];
 char str_CP[6];
 dtostrf(PV,3,2,str_PV);
 dtostrf(CP,3,2,str_CP);
 sprintf(bufferdata,"%s;%s;%d",str_PV,str_CP,now);
 
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
int i=0; //counter for each bite as it arrives
  while (Wire.available()) { 
    bufferparameters[i] = Wire.read(); // every character that arrives it put in order in the empty array "t"
    i=i+1;
  }





  

  
}


void comunicationOUT(){ 

  Wire.write(bufferdata);
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
