// LIBRARIES 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>



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
bool mqtt_in_onoff=false;
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
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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

  if (mqtt_in_onoff){
      uart_send_params();
      uart_receive_data();
      mqtt_send_data();
      if(mqtt_in_getparams){
       mqtt_send_params();} 
    
    }
//  Serial.print(mqtt_in_onoff);
  
//  if(!mqtt_in_onoff)
//    return;
    
 
//  uart_receive_data();
//  uart_send_params();

//  mqtt_send_data();

//  if(mqtt_in_getparams)
//    mqtt_send_params();

    delay(1000);
}


void uart_receive_data(){
  
  if(Serial.available()){

    Pv = Serial.parseFloat();
    Cp = Serial.parseFloat();
    Time = Serial.parseInt();
  }
      
}

void uart_send_params(){

  Serial.println(setp);
  Serial.println(Kp);
  Serial.println(Ti);
  Serial.println(Td);
  Serial.println(on_off);
}

void mqtt_send_data(){
  char data[50];

  sprintf(data, "%f;%f;%f;%ld", setp,Pv,Cp,Time);
  Serial.print("Publish message: ");
  Serial.println(data);  
  client.publish("plant1/data", data);
}

void mqtt_send_params(){
  char data1[50];

  sprintf(data1, "%f;%f;%f;%f;%s", setp,Kp,Ti,Td,on_off);
  Serial.print("Publish message: ");
  Serial.println(data1);
  client.publish("plant1/parameters", data1);
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
  
  if (String(topic) == "plant1/on_off"){
    //Serial.print("Se ejecuto la condicion del topic on_off");

    for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
    }
    inmsg[length] = '\0';
    mqtt_in_onoff = String(inmsg) == "1";
    //Serial.print(mqtt_in_onoff);
   
    
  } else if (String(topic) == "plant1/get_parameters"){
      //Serial.print("Se ejecuto la condicion del topic get params");
      for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
      }
      inmsg[length]='\0';
      mqtt_in_getparams = String(inmsg) == "";
//      Serial.println(inmsg);
//      if (mqtt_in_getparams){
//        Serial.print("recibi un espacio vacio");
//        }
      
  } else if (String(topic) == "plant1/update_parameters") {
      
      //Serial.print("Se ejecuto la condicion del topic update params");  
      for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
      }
      inmsg[length]='\0';
      Serial.print(inmsg);
      ssncanf(inmsg,50,"%f;%f;%f;%f",&setp,&Kp,&Ti,&Td);
      Serial.print("setp: ");
      Serial.print(setp);
//      Serial.print(" Kp: ");
//      Serial.print(Kp);
//      Serial.print(" Td: ");
//      Serial.print(Td);
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
