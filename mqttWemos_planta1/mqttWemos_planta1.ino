// LIBRARIES 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//MQTT COMMUNICATION PARAMETERS
const char* ssid = "WIFI_4C";
const char* password = "chontaduroexternocleidomastoideo";
const char* mqtt_server = "192.168.1.64";
// VARIABLES
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char* mqtt_in_onoff;
char* mqtt_in_getparams;
char* mqtt_in_updtparams;
int value = 0;
float SP=0.00;
float PV=0.00;
float CP=0.00;







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
  MQTT_OUT();
  
  
  
  
  
 
}
void SERIAL_IN(){}
void SERIAL_OUT(){}
void MQTT_IN(){
  client.subscribe("plant1/on_off");
  client.subscribe("plant1/get_parameters");
  client.subscribe("plant1/update_parameters");  
  }
void MQTT_OUT(){
  unsigned long now = 0;
  now = millis();
  snprintf (msg, MSG_BUFFER_SIZE, "%f;%f;%f;%f",SP,PV,CP,now);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("plant1/data", msg);
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
  Serial.print("] ");
  if (topic == "plant1/on_off"){
    for (int i = 0; i < length; i++) {
    inmsg[i] = (char)payload[i];
    }
    inmsg[length]='\0';
    mqtt_in_onoff=inmsg;
    
  } else if (topic == "plant1/get_parameters"){
      
      for (int i = 0; i < length; i++) {
      inmsg[i] = (char)payload[i];
      }
      inmsg[length]='\0';
      mqtt_in_getparams=inmsg;
      
    } else {
        
        for (int i = 0; i < length; i++) {
        inmsg[i] = (char)payload[i];
        }
        inmsg[length]='\0';
        mqtt_in_updtparams=inmsg;
        }
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  
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
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
