// LIBRARIES 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Wire.h"



//MQTT COMMUNICATION PARAMETERS
const char* ssid = "WIFI_4C";//"udcdocencia"
const char* password = "chontaduroexternocleidomastoideo"; //Universidade.2022
const char* mqtt_server = "192.168.1.64";



// VARIABLES
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
bool mqtt_in_onoff;
bool mqtt_in_getparams;
char* mqtt_in_updtparams;
char params[100];
int value = 0;
double setp =30;
double Kp =1;
double Ti =8.14;
double Td =1.02;
double Cp;
double Pv;
int Time;
char on_off;





void setup() {
  
  Serial.begin(115200);
  //setup_wifi();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  Wire.begin();
}

void loop() {

//  if (!client.connected()) {
//    reconnect();
//  }
//  
//  client.loop();

  /*
     1. MQTT: RECEIVE on/off from app
     2. i2c: SEND on/off
     3. i2c: RECEIVE data
     4. i2c: RECEIVE parameters
     5. MQTT: SEND data
     6. MQTT: RECEIVE parameter request
     7. MQTT: SEND parameters (if requested)
     8. MQTT: RECEIVE new parameters
     9: i2c: SEND new parameters
  */

//  if (mqtt_in_onoff){
//      i2c_send_params();
//      i2c_receive_data();
//      mqtt_send_data();
//      
//      
//      if(mqtt_in_getparams){
//       mqtt_send_params();
//       mqtt_in_getparams=false;
//       }
       
    i2c_send_params();
    i2c_receive_data();    
    
    
    
//  Serial.print(mqtt_in_onoff);
  
//  if(!mqtt_in_onoff)
//    return;
    
 
//  i2c_receive_data();
//  i2c_send_params();

//  mqtt_send_data();

//  if(mqtt_in_getparams)
//    mqtt_send_params();

    delay(1000);
}


void i2c_receive_data(){

  int msg_size = 2*sizeof(double) + sizeof(int);
  Wire.requestFrom(2,msg_size);
  byte data[msg_size];
  int index = 0;
  bool received=false;
  
  while(Wire.available()){
    Serial.println("Comienza  a leer");
    *(data + index) = (byte)Wire.read();
    index++;
    Serial.println(index);
    received=true;
  }

  if (!received)
    return;
  
  Pv=*((double*)&data[0]);
  Serial.print("PV: ");
  Serial.println(Pv);
  
  Cp=*((double*)&data[sizeof(double)]);
  Serial.print("CP: ");
  Serial.println(Cp);
  
  Time=*((int*)&data[2*sizeof(double)]);
  Serial.print("Time: ");
  Serial.println(Time);
      
}

void i2c_send_params(){
  Wire.beginTransmission(2);
  Wire.write((byte*)&setp, sizeof(setp));
  Wire.endTransmission();
  Wire.write((byte*)&Kp, sizeof(Kp));
  Wire.endTransmission();
  Wire.write((byte*)&Ti, sizeof(Ti));
  Wire.endTransmission();
  Wire.write((byte*)&Td, sizeof(Td));
  Wire.endTransmission();
  Wire.write((byte*)&on_off, sizeof(on_off));
  Wire.endTransmission();
  Serial.println("Se enviaron los datos");
}

void mqtt_send_data(){
  char data[50];

  sprintf(data, "%f;%f;%f;%ld", setp,Pv,Cp,Time);
  Serial.print("Publish message: ");
  Serial.println(data);  
  client.publish("plant2/data", data);
}

void mqtt_send_params(){
  char data1[50];

  sprintf(data1, "%f;%f;%f;%f;%c", setp,Kp,Ti,Td,on_off);
  Serial.print("Publish message: ");
  Serial.println(data1);
  client.publish("plant2/parameters", data1);
}



void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");

  
}




void callback(char* topic, byte* payload, unsigned int length) {
  
  char inmsg[length+1];
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  
  if (String(topic) == "plant2/on_off"){
    //Serial.print("Se ejecuto la condicion del topic on_off");

    for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
    }
    inmsg[length] = '\0';
    on_off=inmsg[0];
    mqtt_in_onoff = String(inmsg) == "1";
    //Serial.print(mqtt_in_onoff);
   
    
  } else if (String(topic) == "plant2/get_parameters"){
      //Serial.print("Se ejecuto la condicion del topic get params");
      for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
      }
      inmsg[length]='\0';
      mqtt_in_getparams = String(inmsg) == " ";

     
      
  } else if (String(topic) == "plant2/update_parameters") {
    float inc_setp,inc_kp,inc_ti,inc_td;
      
      //Serial.print("Se ejecuto la condicion del topic update params");  
      for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
      }
      inmsg[length]='\0';
      //Serial.print(inmsg);
      sscanf(inmsg,"%f;%f;%f;%f",&inc_setp,&inc_kp,&inc_ti,&inc_td);
           
      setp=inc_setp;
      Kp=inc_kp;
      Ti=inc_ti;
      Td=inc_td;
      
  }
  
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("plant2/on_off");
      client.subscribe("plant2/get_parameters");
      client.subscribe("plant2/update_parameters");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
