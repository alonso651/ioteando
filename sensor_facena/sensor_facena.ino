
/*
Conexiones:
  BH1750
    VCC -> 3V3
    GND -> GND
    SCL -> SCL (GPIO5)
    SDA -> SDA (GPIO4)

  DHT11
    VCC -> 3V3
    GND -> GND
    DATA -> GPIO2

  FACTORY RESET
    INTERRUPCION -> GPIO14
*/

//LIBRERIAS
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <BH1750.h>


BH1750 lightMeter;
DHTesp dht;
WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;

//VARIABLES DE MQTT
char mqtt_server[40] = "sambrana.com.ar";
char mqtt_port[6] = "1883";
char suscribe_topic[34] = "comando";
char publish_topic[34] = "facena";
long loopTime = 2000;
//VARIABLES
bool shouldSaveConfig = false;
long lastMsg = 0;

// función para reinicio de las credenciales del WiFi
void servicioboton(){
  WiFi.disconnect();
}
//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// función para reinicio de las credenciales del WiFi
void factoryReset(){
  WiFi.disconnect();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String dato="";
  char c;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    dato += c;
  }
  Serial.println();
  Serial.print("------json------");Serial.println(dato);
  dato = "";


}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Conectando con servidor MQTT...");
    if (client.connect("Sensor_facena")) {
      Serial.println("conectado");
      client.publish(publish_topic, "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(14, INPUT);     //GPIO14
  attachInterrupt(digitalPinToInterrupt(14),servicioboton,RISING);
  Serial.begin(115200);
  setup_wifiManager();
  setup_OTA();  
  setup_wifi();
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);
  dht.setup(D4, DHTesp::DHT11);
  Wire.begin();
  lightMeter.begin();
}
void loop() {
  ArduinoOTA.handle();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > loopTime) {
    lastMsg = now; 
    Serial.print("Publish message: ");
    Serial.println(json_sensores());
    
    char msg[200];
    json_sensores().toCharArray(msg, 200);
    //Publicamos a MQTT server
    client.publish(publish_topic, msg);
  }
}
