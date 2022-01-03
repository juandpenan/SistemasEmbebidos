 //.......................LIBRARIES....................................//
#include <PID_v1.h>         // PID´s library
#include <stdio.h>          // C++ library for snprintf
#include "SerialTransfer.h" // Serial communication library
//......................DEFINITIONS...................................//
#define InputPin A0 // Level input (0-10V) 
#define OutputPin 10 // Pump´s angular velocity output (0-10V) 
//...................... VARIABLES....................................//
SerialTransfer myTransfer;
double PV=0;   // Process value 
double setp=10;   // Set point
double ERR=0;  // Error
double CP=0;   // Control process
int Ts=1000;   // Sample time 1s
double Kp=1;   // Proportional constant
double Ti=8.14;   // Integrative constant
double Td=1.02;   // Derivative constant
char bufferinfo[50]; 
float times=0;

//...................... PID ..........................................//

PID myPID(&PV, &CP, &setp, Kp, Ti, Td, DIRECT);

void setup() {
// PID settings
myPID.SetMode(AUTOMATIC);
// Communication settings
Serial.begin(115200);
myTransfer.begin(Serial);



}

void loop() {

//PID();
comunicationOUT();


}

void comunicationIN(){}
void comunicationOUT(){
uint16_t sendSize = 0;
//float(SP,PV,CP);
snprintf(bufferinfo,50,"%f;%f;%f;%f",setp,PV,CP,times);
sendSize = myTransfer.txObj(bufferinfo, sendSize);
myTransfer.sendData(sendSize);

}

void PID() {
 myPID.SetSampleTime(Ts);
 myPID.SetTunings(Kp,Ti,Td);
 PV= analogRead(InputPin);
 myPID.Compute();
 analogWrite(OutputPin,CP); 
}
