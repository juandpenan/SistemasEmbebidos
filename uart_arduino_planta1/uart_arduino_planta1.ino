 //.......................LIBRARIES....................................//
#include <PID_v1.h>         // PID´s library
#include <stdio.h>          // C++ library for snprintf
#include <string.h>

//......................DEFINITIONS...................................//
#define InputPin A0 // Level input (0-10V) 
#define OutputPin 3 // Pump´s angular velocity output (0-10V) 

//...................... VARIABLES....................................//
double PV;   // Process value 
double setp;   // Set point
double ERR=0;  // Error
double CP;   // Control process
int Ts=1000;   // Sample time 1s
double Kp;   // Proportional constant
double Ti;   // Integrative constant
double Td;   // Derivative constant
char bufferparameters[50]; 
char bufferdata[50]; 
char uart_on_off;


//...................... PID ..........................................//
PID myPID(&PV, &CP, &setp, Kp, Ti, Td, DIRECT);


void setup() {
  // PID settings
  myPID.SetMode(AUTOMATIC);
  // Communication settings
  Serial.begin(115200);
}

int now = 0;

bool can_send = true;

void loop() {

  unsigned long StartTime = millis();

  comunicationIN();
  

  if (uart_on_off == '1'){
     PID();
    }
    else {
     CP=0;
   }  
  comunicationOUT();
  
  

  
  
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  
  now = ElapsedTime /1000;
  if (now<1){
    now=1;
    }

  delay(1000);
}

void comunicationIN(){
    if(Serial.available())
      Serial.read();
    if(Serial.available())
      setp=Serial.parseFloat();
    if(Serial.available())
      Kp=Serial.parseFloat();
    if(Serial.available())
      Ti=Serial.parseFloat();
    if(Serial.available())
      Td=Serial.parseFloat();
    if(Serial.available())
      uart_on_off=Serial.read();
    Serial.println('k');
}

void comunicationOUT(){

   while(!can_send){}
   can_send = false;
   
   double cp_aux;
   cp_aux=CP*(100.0/255.0);
   Serial.println(PV);
   Serial.println(cp_aux);
   Serial.println(now);  

    wait_for_ok();
}


void wait_for_ok(){

   while(!Serial.available()){}

   char ok = Serial.read();
   can_send = true;
}
void PID() {
  
  myPID.SetSampleTime(Ts);
  myPID.SetTunings(Kp,Ti,Td);
  
  double tmp =analogRead(InputPin);
  
  PV= (tmp*(100/1023.0));
  
  
  myPID.Compute();
  
  analogWrite(OutputPin,CP);
  //
}
