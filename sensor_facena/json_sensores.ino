String json_sensores(){
    //Lectura de los sensores
    float Humedad = dht.getHumidity();
    float Temperatura = dht.getTemperature();
    float lux = lightMeter.readLightLevel();
    IPAddress ip = WiFi.localIP();

    //Transformación de tipos - float -> Char
    char hum[8];
    dtostrf(Humedad, 6, 2, hum); 
    char tem[8];
    dtostrf(Temperatura, 6, 2, tem); 
    char luz[8];
    dtostrf(lux, 6, 2, luz); 
    String ipLocal="";
    for (int i=0; i<4; i++) {
      ipLocal += i  ? "." + String(ip[i]) : String(ip[i]);
    } 

    //Construcción del Objeto JSON
    StaticJsonBuffer<250> doc;
    JsonObject& root = doc.createObject();
    root["nodo"] = String(ESP.getChipId());
    root["ssid"] = String(wifiManager.getSSID());
    root["pass"] = String(wifiManager.getPassword());
    root["localIP"] = ipLocal;
    root["temp"] = tem;
    root["hum"] = hum;
    root["luz"] = luz;
    String output;
    root.printTo(output);
    return output;
}