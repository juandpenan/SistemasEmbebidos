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

char uart_on_off='1';


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

//struct Data {
//  double SetP;
//  double PV;
//  double CP;
//  int Time;
//  double KP;
//  double TI;
//  double TD;
//  char* ON_OFF;
//} send_params;

void loop() {
  comunicationIN();
  
  if (uart_on_off != '0'){
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

    
    if(Serial.available()){

      char buffer_data[100];

//      Serial.readBytes(buffer_data, 100);
//      sscanf(buffer_data, "%f;%f;%f;%f;%f;%f;%s", &setp, &PV, &CP, &Kp, &Ti, &Td, &uart_on_off);
      setp=Serial.parseFloat();
      //PV=Serial.parseFloat();
      //CP=Serial.parseFloat();
      Kp=Serial.parseFloat();
      Ti=Serial.parseFloat();
      Td=Serial.parseFloat();
      uart_on_off=Serial.read();
//      Serial.print("recibiendo: ");
//      Serial.println(buffer_data);
    }  
    
}

void comunicationOUT(){ 
//  send_params.SetP = setp;
//  send_params.PV = PV;
//  send_params.CP = CP;
//  send_params.Time = now;
//  send_params.KP = Kp;
//  send_params.TI = Ti;
//  send_params.TD = Td;
//  send_params.ON_OFF = uart_on_off;

//  myTransfer.sendDatum(send_params);

//  char buffer_data[100];
//  sprintf(buffer_data, "%f;%f;%f;%f;%f;%f;%d", setp, PV, CP, Kp, Ti, Td, now);
//  Serial.print("SENDING: ");
//  Serial.println(buffer_data);
   //Serial.println(setp);
   Serial.println(PV);
   Serial.println(CP);
//   Serial.println(Kp);
//   Serial.println(Ti);
//   Serial.println(Td);
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
