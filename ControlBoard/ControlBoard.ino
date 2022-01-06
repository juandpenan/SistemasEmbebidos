 //.......................LIBRARIES....................................//
#include <PID_v1.h>         // PID´s library
#include <stdio.h>          // C++ library for snprintf
#include "SerialTransfer.h" // Serial communication library
#include <string.h>
//......................DEFINITIONS...................................//
#define InputPin A0 // Level input (0-10V) 
#define OutputPin 3 // Pump´s angular velocity output (0-10V) 
//...................... VARIABLES....................................//
SerialTransfer myTransfer;
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

int now = 0;

struct Data {
  double SetP;
  double PV;
  double CP;
  int Time;
  double KP;
  double TI;
  double TD;
  char* ON_OFF;
} send_params;

void loop() {
  comunicationIN();
  
  if (uart_on_off != "0"){
     PID();
     //FSerial.print("se ejecuto el PID");
  } else {
    CP=0;
  }
  
  comunicationOUT();

  delay(1000);
  now++;
}

void comunicationIN(){

    if(myTransfer.available()){
      myTransfer.rxObj(send_params);
  
      setp = send_params.SetP;
      PV = send_params.PV;
      CP = send_params.CP;
      Kp = send_params.KP;
      Ti = send_params.TI;
      Td = send_params.TD;
      uart_on_off = send_params.ON_OFF;
      Serial.print("recibiendo: ");
      Serial.print(setp);
    }  
    
}

void comunicationOUT(){ 
  uint16_t sendSize = 0;
  uint16_t sendSize1 = 0;

  send_params.SetP = setp;
  send_params.PV = PV;
  send_params.CP = CP;
  send_params.Time = now;
  send_params.KP = Kp;
  send_params.TI = Ti;
  send_params.TD = Td;
  send_params.ON_OFF = uart_on_off;

  myTransfer.sendDatum(send_params);
  Serial.print("SENDING: ");
  Serial.println((int)send_params.Time);

}

void PID() {
 myPID.SetSampleTime(Ts);
 myPID.SetTunings(Kp,Ti,Td);
 double tmp =analogRead(InputPin);
 PV= (tmp*(100/1023.0));
 myPID.Compute();
 analogWrite(OutputPin,CP);
}
