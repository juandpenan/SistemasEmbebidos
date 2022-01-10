// LIBRARIES 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>



//MQTT COMMUNICATION PARAMETERS
#define MSG_BUFFER_SIZE  (50)

const char* ssid = "udcdocencia"; // "WIFI_4C";//
const char* password = "Universidade.2022"; // "chontaduroexternocleidomastoideo"; //
const char* mqtt_server = "10.20.30.242";

WiFiClient espClient;
PubSubClient client(espClient);


// VARIABLES
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
bool mqtt_in_onoff;
bool mqtt_in_getparams;
char* mqtt_in_updtparams;
char params[100];
double setp =30;
double Kp =1;
double Ti =8.14;
double Td =1.02;
double Cp;
double Pv;
int Time;
char on_off;


bool can_send = true;


void setup() {
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  // Assure connection
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  if (mqtt_in_onoff){
    uart_send_params();
    uart_receive_data();
    mqtt_send_data();
    
    
    if(mqtt_in_getparams){
      mqtt_send_params();
      mqtt_in_getparams=false;
    }  
  }
  
  delay(1000);
  
}


void uart_receive_data(){
  if(Serial.available())
    Serial.read(); 
  if(Serial.available())
    Pv = Serial.parseFloat();
  if(Serial.available())
    Cp = Serial.parseFloat();
  if(Serial.available())
    Time = Serial.parseInt();

  Serial.println('k');
}

void uart_send_params(){
  while(!can_send){}
  can_send=false;
  Serial.println('s');
  Serial.println(setp);
  Serial.println(Kp);
  Serial.println(Ti);
  Serial.println(Td);
  Serial.println(on_off);

  wait_for_ok();
}

void wait_for_ok(){

   while(!Serial.available()){}

   char ok = Serial.read();
   can_send = true;
}

void mqtt_send_data(){
  char data[50];

  sprintf(data, "%f;%f;%f;%ld", setp,Pv,Cp,Time);
//  Serial.print("Publish message: ");
//  Serial.println(data);  
  client.publish("plant1/data", data);
}

void mqtt_send_params(){
  char data1[50];

  sprintf(data1, "%f;%f;%f;%f;%c", setp,Kp,Ti,Td,on_off);
//  Serial.print("Publish message: ");
//  Serial.println(data1);
  client.publish("plant1/parameters", data1);
}



void setup_wifi() {

  delay(10);
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }

  randomSeed(micros());

//  Serial.println("");
//  Serial.println("WiFi connected");
  
}




void callback(char* topic, byte* payload, unsigned int length) {
  
  char inmsg[length+1];
  
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.println("] ");
  
  if (String(topic) == "plant1/on_off"){
    //Serial.print("Se ejecuto la condicion del topic on_off");

    for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
    }
    inmsg[length] = '\0';
    on_off=inmsg[0];
    mqtt_in_onoff = String(inmsg) == "1";
    //Serial.print(mqtt_in_onoff);
   
    
  } else if (String(topic) == "plant1/get_parameters"){
      //Serial.print("Se ejecuto la condicion del topic get params");
      for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
      }
      inmsg[length]='\0';
      mqtt_in_getparams = String(inmsg) == " ";

     
      
  } else if (String(topic) == "plant1/update_parameters") {
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
  
}



void reconnect() {
  
  // Loop until we're reconnected
  while (!client.connected()) {
    
//    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
  
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
//      Serial.println("connected");
      client.subscribe("plant1/on_off");
      client.subscribe("plant1/get_parameters");
      client.subscribe("plant1/update_parameters");
    } else {
//      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
