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

void loop() {


comunicationIN();
if (uart_on_off != '0')
  {
   PID();
   //FSerial.print("se ejecuto el PID");
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
  now = millis() / 1000.0;
//  snprintf(bufferdata,50,"%f;%f;%f;%f",setp,PV,CP,now);
sprintf(bufferdata,"%d;%d;%d;%ld",(int)setp,(int)PV,(int)CP,(long long)now);
// CAMBIA A PARAMETROS
  snprintf(bufferparameters,50,"%d;%d;%d;%d;%d",(int)setp,(int)Kp,(int)Ti,(int)Td,(int)uart_on_off);
  sendSize = myTransfer.txObj(bufferdata, sendSize);
  sendSize1 = myTransfer.txObj(bufferparameters, sendSize1);
  myTransfer.sendData(sendSize);
  myTransfer.sendData(sendSize1);
  Serial.print("SENDING: ");
  Serial.println(bufferdata);

}

void PID() {
 myPID.SetSampleTime(Ts);
 myPID.SetTunings(Kp,Ti,Td);
 double tmp =analogRead(InputPin);
 PV= (tmp*(100/1023.0));
 myPID.Compute();
// Serial.print("- Senal control: ");
// Serial.println(CP);
 analogWrite(OutputPin,CP);
// analogWrite(OutputPin, 255); 
// Serial.print("Senal sensor: ");
// Serial.print(PV);
// Serial.print(" - set point: ");
// Serial.print(setp);
// Serial.print("- Senal control: ");
// Serial.println(CP);
}
