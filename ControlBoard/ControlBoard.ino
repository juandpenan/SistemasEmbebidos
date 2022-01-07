 //.......................LIBRARIES....................................//
#include <PID_v1.h>         // PID´s library
#include <stdio.h>          // C++ library for snprintf
#include <string.h>
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
float times=0;
char uart_on_off='1';


//...................... PID ..........................................//

PID myPID(&PV, &CP, &setp, Kp, Ti, Td, DIRECT);

void setup() {
// PID settings
myPID.SetMode(AUTOMATIC);
// Communication settings
Serial.begin(115200);

}

int now = 0;


void loop() {
  comunicationIN();
  
  if (uart_on_off != '0'){
     PID();
    } else {
    CP=0;
  }  
  comunicationOUT();

  delay(1000);
  now++;
}

void comunicationIN(){

    
    if(Serial.available()){
      setp=Serial.parseFloat();
      Kp=Serial.parseFloat();
      Ti=Serial.parseFloat();
      Td=Serial.parseFloat();
      uart_on_off=Serial.read();
    }  
    
}

void comunicationOUT(){ 
   Serial.println(PV);
   Serial.println(CP);
   Serial.println(now);  

}

void PID() {
 myPID.SetSampleTime(Ts);
 myPID.SetTunings(Kp,Ti,Td);
 double tmp =analogRead(InputPin);
 PV= (tmp*(100/1023.0));
 myPID.Compute();
 analogWrite(OutputPin,CP);
}
