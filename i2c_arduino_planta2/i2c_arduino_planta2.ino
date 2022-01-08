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
char i2c_on_off='1';


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
void Parseinfo(){
 char str_PV[6];
 char str_CP[6];
 dtostrf(PV,3,2,str_PV);
 dtostrf(CP,3,2,str_CP);
 sprintf(bufferdata,"%s;%s;%d",str_PV,str_CP,now);
 int setp_int;
 float kp_int,ti_int,td_int;
 char on_off;
 char setpoint[6];
 char s_setp[6];
 char *s_kp;
 int index;
 String s_bufferparameters; 
 for(int i =0;i < sizeof(bufferparameters); i++){
  
  if(bufferparameters[i] != ";"){
    s_setp[i]=bufferparameters[i];
    
    }
  }
  Serial.println(s_setp);
  
// s_setp = strtok(bufferparameters, ";");
// 
// s_bufferparameters=String(bufferparameters).remove(0,s_setp.length());
// Serial.println(s_bufferparameters);
// sscanf(bufferparameters,"%s;%f;%f;%f;%c",setpoint,&kp_int,&ti_int,&td_int,&on_off);
// //setp=setp_int;
// setp=String(bufferparameters).toDouble();
// Kp=String(bufferparameters[sizeof(setp)]).toDouble();
// //Kp=kp_int;
// Ti=ti_int;
// Td=td_int;
// Serial.print("Info recibida: ");
// Serial.print(String(bufferparameters));
// Serial.print("Setpoint recibido: ");
// Serial.print(setp);
// Serial.print(" Kp ");
// Serial.println(Kp);

 
 }


void comunicationIN(int bytes){
int i=0; //counter for each bite as it arrives
  while (Wire.available()) { 
    bufferparameters[i] = Wire.read(); // every character that arrives it put in order in the empty array "t"
    i=i+1;
  }





  
//  int msg_size = 4*sizeof(double) + 1;
//
//  byte receiver[msg_size];
//
//  int i;
//  for(i=0; i<msg_size; i++)
//    *(receiver + i) = (byte)Wire.read();
//
//  setp=*((double*)&receiver);
//  Serial.print("setp: ");
//  Serial.println(setp);
//  
//  Kp=*((double*)&receiver[sizeof(double)]);
//  Serial.print("Kp: ");
//  Serial.println(Kp);
//  
//  Ti=*((double*)&receiver[2*sizeof(double)]);
//  Serial.print("Ti: ");
//  Serial.println(Ti);
//  
//  Td=*((double*)&receiver[3*sizeof(double)]);
//  Serial.print("Td: ");
//  Serial.println(Td);
//  
//  i2c_on_off=*((char*)&receiver[3*sizeof(double) + 1]);
//  Serial.print("i2c_on_off: ");
//  Serial.println(i2c_on_off);
  
}


void comunicationOUT(){ 

//   Wire.write((byte*)&PV, sizeof(PV));
//   Wire.write((byte*)&CP, sizeof(CP));
//   Wire.write((byte*)&now, sizeof(now));
//   Serial.println("Datos enviados");
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
