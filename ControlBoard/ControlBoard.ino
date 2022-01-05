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
if (uart_on_off != "0")
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


struct Data {
  double SetP;
  double PV;
  double CP;
  long long Time;
} send_data;

struct Params {
  double SetP;
  double KP;
  double TI;
  long long TD;
  char* ON_OFF;
} send_params;

void comunicationOUT(){
  long long now = 0; // time in seconds 
  uint16_t sendSize = 0;
  uint16_t sendSize1 = 0;
  now = millis() / 1000.0;

  send_data.SetP = setp;
  send_data.PV = PV;
  send_data.CP = CP;
  send_data.Time = now;

  send_params.SetP = setp;
  send_params.KP = Kp;
  send_params.TI = Ti;
  send_params.TD = Td;
  send_params.ON_OFF = uart_on_off;
  
//  snprintf(bufferdata,50,"%f;%f;%f;%f",setp,PV,CP,now);
//sprintf(bufferdata,"%d;%d;%d;%ld",(int)setp,(int)PV,(int)CP,(long long)now);
// CAMBIA A PARAMETROS
//  snprintf(bufferparameters,50,"%d;%d;%d;%d;%d",(int)setp,(int)Kp,(int)Ti,(int)Td,(int)uart_on_off);
//  sendSize = myTransfer.txObj(bufferdata, sendSize);
//  sendSize1 = myTransfer.txObj(bufferparameters, sendSize1);

  myTransfer.sendDatum(send_data);
  myTransfer.sendDatum(send_params);
//  Serial.print("SENDING: ");
//  Serial.println(send_data);

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
