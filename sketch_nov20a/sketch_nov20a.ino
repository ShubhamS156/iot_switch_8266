#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define NUM_CHANNEL 4

// Update these with values suitable for your network.

typedef struct Switch{
  int id;
  int pin;
  int state;
};

Switch switches[NUM_CHANNEL];
int SwitchPinMap[NUM_CHANNEL] = {1,2,3,4};

const char* mqtt_server = "mqtt.archeindustries.com";
String mqtt_topic = String(String("SWITCH_")+String(ESP.getChipId()));
WiFiClient espClient;
WiFiManager wm;
PubSubClient client(espClient);



//[switch id] [state] 
void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr(topic);
  if(topicStr == mqtt_topic){
    int id = payload[0];
    int state = payload[1];
    updateSwitchState(id,state);
  }
  else{
    Serial.println("Unknown TOPIC");
  }
}



void setup() {
   pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
   Serial.begin(115200);
   initSwitches();
   bool res;
    res = wm.autoConnect(mqtt_topic.c_str());
    if(!res){
      Serial.println("Failed to Connect");
      ESP.restart();
    }
    else{
      Serial.println("Connected to WiFi");
    }
    
    randomSeed(micros());
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    String clientID = "SWITCH_";
    clientID += String(random(0xffff),HEX);

     if(client.connect(clientID.c_str())){
      Serial.println("Connected TO MQTT"); 
      client.subscribe(mqtt_topic.c_str());
    }
    
}

void loop() {
}

/*
 * Gives id to each switch
 * Assigns pins , set pinMode and Turn OFF every channel.
 */
void initSwitches(){
  for(int i=0; i<NUM_CHANNEL; i++){
    switches[i].id = i;
    switches[i].pin = SwitchPinMap[i];
    switches[i].state = 0;
    pinMode(switches[i].pin,OUTPUT);
    digitalWrite(switches[i].pin,LOW);
  }
}

/* Takes switch id and updates the state-> HIGH or LOW */
void updateSwitchState(int id, int state){
  switches[id].state = state; //update the state.
  digitalWrite(switches[id].pin ,state); //set the state of phy switch.
}
