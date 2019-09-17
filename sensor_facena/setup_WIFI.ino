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

// función para reinicio de las credenciales del WiFi
void factoryReset(){
  WiFi.disconnect();
}