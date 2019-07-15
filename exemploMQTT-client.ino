#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include <Servo.h>


//WiFi
const char* SSID = "brisa-322556"; 
const char* PASSWORD = "9la2o3le"; 
WiFiClient wifiClient;     
Servo myservo; 
                   
 
//MQTT Server
const char* BROKER_MQTT = "iot.eclipse.org"; 
int BROKER_PORT = 1883; 

#define ID_MQTT  "BCI2110"  
#define TOPIC_SUBSCRIBE "SERVO"  
PubSubClient MQTT(wifiClient); 


void waitingConnection();
void initWiFi();
void initMQTT();
void receivePackage(char* topic, byte* payload, unsigned int length);

void setup() {      
  myservo.attach(D10); 
  Serial.begin(115200);

  initWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);  
  MQTT.setCallback(receivePackage); 
}

void loop() {
  waitingConnection();
  MQTT.loop();
}

void waitingConnection() {
    if (!MQTT.connected()) {
       initMQTT(); 
    }
    
    initWiFi();
}

void initWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); 
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void initMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(TOPIC_SUBSCRIBE);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}

void receivePackage(char* topic, byte* payload, unsigned int length) 
{
    int msg = 0;
    for(int i = 0; i < length; i++) 
    {
       int c = (char)payload[i];
       msg = (msg * 10) + (c - 48);
    }

     Serial.println(msg);
     myservo.write(msg); 
}

