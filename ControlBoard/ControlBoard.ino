 //.......................LIBRARIES....................................//
#include <PID_v1.h>         // PID´s library
#include <stdio.h>          // C++ library for snprintf
#include "SerialTransfer.h" // Serial communication library
#include <string.h>
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
char bufferparameters[50]; 
char bufferdata[50]; 
float times=0;

char* uart_on_off;


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


comunicationIN();
if (uart_on_off != '0')
  {
   PID();
  } else {
    CP=0;
    }
comunicationOUT();


}

void comunicationIN(){
  char uartinmsg[50];

  if(myTransfer.available())
  {
    uint16_t recSize = 0;   
    recSize = myTransfer.rxObj(uartinmsg, recSize);    
  }

  if(strlen(uartinmsg)>1){
    
    sscanf(uartinmsg,"%f;%f;%f;%f",&setp,&Kp,&Ti,&Td);

    } else {
    uart_on_off=uartinmsg;      
    }
    
    
}

void comunicationOUT(){
unsigned long now = 0; // time in seconds 
uint16_t sendSize = 0;
uint16_t sendSize1 = 0;
now = millis() * 1000.0;
snprintf(bufferdata,50,"%f;%f;%f;%f",setp,PV,CP,now);
snprintf(bufferparameters,50,"%f;%f;%f;%f",setp,PV,CP,now);
sendSize = myTransfer.txObj(bufferdata, sendSize);
sendSize1 = myTransfer.txObj(bufferparameters, sendSize1);
myTransfer.sendData(sendSize);
myTransfer.sendData(sendSize1);

}

void PID() {
 myPID.SetSampleTime(Ts);
 myPID.SetTunings(Kp,Ti,Td);
 PV= analogRead(InputPin);
 myPID.Compute();
 analogWrite(OutputPin,CP); 
}
