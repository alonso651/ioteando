
/*
Conexiones:

    VCC -> 3V3 o 5V
    GND -> GND
    SCL -> SCL (D1)
    SDA -> SDA (D2)
    ADD -> (not conectar)

*/

//Librerias
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

// const char* mqtt_server = "10.40.60.12";
// const char* topic_suscribe = "comandos";
// const char* topic_publish = "facena";
char mqtt_server[40] = "sambrana.com.ar";
char mqtt_port[6] = "1883";
char suscribe_topic[34] = "comando";
char publish_topic[34] = "facena";

//flag for saving data
bool shouldSaveConfig = false;

long lastMsg = 0;
long loopTime = 2000;

WiFiManager wifiManager;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// función para reinicio de las credenciales del WiFi
void servicioboton(){
  //reset saved settings
  //wifiManager.resetSettings();
  WiFi.disconnect();
  //delay(500);
  //ESP.reset();
}

void callback(char* topic, byte* payload, unsigned int length) {

  String dato="";
  char c;

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
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
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Sensor_bunker")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(publish_topic, "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  pinMode(14, INPUT);
  attachInterrupt(digitalPinToInterrupt(14),servicioboton,RISING);

  Serial.begin(115200);

  //WiFiManager
  setup_wifiManager();
  setup_OTA();
  
  setup_wifi();
  int port = atoi(mqtt_port);
  client.setServer(mqtt_server, port);
  client.setCallback(callback);


  dht.setup(D4, DHTesp::DHT11);
  
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
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
    
    //Lectura de los sensores
    float Humedad = dht.getHumidity();
    float Temperatura = dht.getTemperature();
    float lux = lightMeter.readLightLevel();

    //Transformación de tipos a Char
    char hum[8];
    dtostrf(Humedad, 6, 2, hum); 
    char tem[8];
    dtostrf(Temperatura, 6, 2, tem); 
    char luz[8];
    dtostrf(lux, 6, 2, luz); 
    IPAddress ip = WiFi.localIP();
    String ipLocal="";
    for (int i=0; i<4; i++) {
      ipLocal += i  ? "." + String(ip[i]) : String(ip[i]);
    } 

    //Construcción del Objeto JSON
    StaticJsonBuffer<200> doc;
    JsonObject& root = doc.createObject();
    root["Nodo"] = String(ESP.getChipId());
    root["localIP"] = ipLocal;
    root["temp"] = tem;
    root["hum"] = hum;
    root["luz"] = luz;
    String output;
    root.printTo(output);
    
    Serial.print("Publish message: ");
    Serial.println(output);
    char msg[200];
    output.toCharArray(msg, 200);
    client.publish(publish_topic, msg);
  }
}
