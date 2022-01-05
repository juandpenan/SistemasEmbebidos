// LIBRARIES 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "SerialTransfer.h"

//MQTT COMMUNICATION PARAMETERS
const char* ssid = "udcdocencia";
const char* password = "Universidade.2022";
const char* mqtt_server = "10.20.30.242";

// UART COMMUNICATION
SerialTransfer uart_transfer;

// VARIABLES
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
bool mqtt_in_onoff=true;
bool mqtt_in_getparams;
char* mqtt_in_updtparams;
char data[20];
char params[20];
int value = 0;
float SP=0.00;
float PV=0.00;
float CP=0.00;



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



void setup() {
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  uart_transfer.begin(Serial);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  /*
     1. MQTT: RECEIVE on/off from app
     2. UART: SEND on/off
     3. UART: RECEIVE data
     4. UART: RECEIVE parameters
     5. MQTT: SEND data
     6. MQTT: RECEIVE parameter request
     7. MQTT: SEND parameters (if requested)
     8. MQTT: RECEIVE new parameters
     9: UART: SEND new parameters
  */

  if(!mqtt_in_onoff)
    return;
    
  uart_send_on_off();
  uart_receive_data();
  uart_receive_params();
  mqtt_send_data();

  if(mqtt_in_getparams)
    mqtt_send_params();

  
  
}

void uart_send_on_off(){
  uint16_t sizeData = 0;
  sizeData =  uart_transfer.txObj(mqtt_in_onoff, sizeData);
//  uart_transfer.sendData(sizeData);
}


void uart_receive_data(){
  if(!uart_transfer.available())
    return;

  uart_transfer.rxObj(send_data);
  Serial.println("DATA: ");
  Serial.println(send_data.SetP);
  Serial.println(send_data.PV);
}

void uart_receive_params(){
  if(!uart_transfer.available())
    return;

  uart_transfer.rxObj(send_params);
  Serial.println("PARAMS: ");
  Serial.println(send_data.SetP);
  Serial.println(send_data.KP);
}

void uart_send_params(){
  uint16_t sizeData = 0;
  sizeData =  uart_transfer.txObj(params, sizeData);
//  uart_transfer.sendData(sizeData);
}

void mqtt_send_data(){
  Serial.print("Publish message: ");
  Serial.println(data);
  client.publish("plant1/data", data);
}

void mqtt_send_params(){
  Serial.print("Publish message: ");
  Serial.println(params);
  client.publish("plant1/parameters", params);
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
  
  if (topic == "plant1/on_off"){
    for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
    }
    inmsg[length] = '\0';
    mqtt_in_onoff = inmsg == "1";
    //mqtt_in_onoff = true;
    
  } else if (topic == "plant1/get_parameters"){
      
      for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
      }
      inmsg[length]='\0';
      mqtt_in_getparams = inmsg == "";
      
  } else {
        
      for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
      }
      inmsg[length]='\0';
      mqtt_in_updtparams = inmsg;
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
      client.subscribe("plant1/on_off");
      client.subscribe("plant1/get_parameters");
      client.subscribe("plant1/update_parameters");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
