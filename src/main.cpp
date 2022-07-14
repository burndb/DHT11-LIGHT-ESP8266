#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "NETGEAR";
const char* password = "";

#define DHTPIN D3    // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

String HOST_NAME = "http://192.168.211.56"; // apache server
String PATH_NAME   = "/insert_temp.php?";  //php script
String queryTemp = "temperature=";
String queryHum = "humidity=";
String queryLight = "light=";
String andOperator = "&";
WiFiClient wifiClient;
HTTPClient http;
// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
int l = 0;

unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

void post_readings() {
  http.begin(wifiClient, (HOST_NAME + PATH_NAME + queryTemp + t + andOperator + queryHum + h + andOperator + queryLight  + l)); //HTTP
  Serial.println(HOST_NAME + PATH_NAME + queryTemp + t + andOperator + queryHum + h + andOperator + queryLight  + l);
  int httpCode = http.GET();
  if(httpCode > 0) {
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } 
    else {
      // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } 
  else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
      http.end();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    //read and print light value
    l = (int)analogRead(A0);
    Serial.println("Lichtwert ADC: ");
    Serial.println(l);

    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println("Temperature: ");
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println("Humidity: ");
      Serial.println(h);
    }
    post_readings();
  }
}