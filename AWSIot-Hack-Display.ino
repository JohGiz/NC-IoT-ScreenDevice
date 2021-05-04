#include "config.h"
#include "images.h"
#include "FS.h"
#include "SSD1306Wire.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* AWS_endpoint = AWS_IOT_ENDPOINT;

long lastMsg = 0;
char msg[50];
int value = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
DynamicJsonDocument doc(1024);
JsonArray arr; // = doc.as<JsonArray>();
SSD1306Wire display(0x3c, 5, 4);

void callback(char* topic, byte* inData, unsigned int length) {

  Serial.println("We have message incoming");
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print(" ");
  Serial.println(length);

  String strData;
  for (int i = 0; i < length; i++) {
    // Serial.print((char)inData[i]);
    strData += (char)inData[i];
  }

  deserializeJson(doc, strData);
  serializeJsonPretty(doc, Serial);
  arr = doc.as<JsonArray>();
}

WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, callback, espClient); //set  MQTT port number to 8883 as per //standard


void setup_wifi() {
  delay(10);
  
  // We start by connecting to a WiFi network
  espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  while(!timeClient.update()){
    timeClient.forceUpdate();
  }

  espClient.setX509Time(timeClient.getEpochTime());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(AWS_IOT_DEVICE_ID)) {
      Serial.println("connected");
      client.publish(AWS_IOT_PUBLISH_ALIVE, "{ \"message\": \"IoT Connected:  \""+ AWS_IOT_DEVICE_ID +" } ");
      client.subscribe(AWS_IOT_SUBSCRIPTION);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      espClient.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  display.init();
  display.drawXbm(25, 0, NC_CLOUD_small_white_x64_width, NC_CLOUD_small_white_x64_height, NC_CLOUD_small_white_x64_bits);
  display.display();

  setup_wifi();
  delay(1000);
  
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");

  delay(1000);

  if (espClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Failed to open private cert file");
  }
  else
    Serial.println("Success to open private cert file");

  delay(1000);

  if (espClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");

  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
  if (!ca) {
    Serial.println("Failed to open ca ");
  }
  else
    Serial.println("Success to open ca");

  delay(1000);

  if(espClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
}



void loop() {

  if (!client.connected()) {
    reconnect();
  }

  // Loop existing sc
  for (JsonVariant itemObj : arr) {    
    display.clear();
    int screenDelay = 1000;
    if (itemObj["type"] == "article") {
      Serial.println("Article printing...");
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 10, itemObj["preheadline"]);
      display.setFont(ArialMT_Plain_24);
      display.drawString(0, 25, itemObj["headline"]);
      screenDelay = itemObj["delay"];
    } 
    else if (itemObj["type"] == "image") {  
      display.drawXbm(0, 0, itemObj["width"], itemObj["height"], itemObj["data"]);
      Serial.println("Print image");
      screenDelay = itemObj["delay"];  
    } 
    else {
      Serial.println("Unknown type...");
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 0, "Unknown message type");
      display.drawString(0, 10, "received as MQTT message");
      screenDelay = itemObj["delay"];
    }    
    display.display();
    delay(screenDelay);
  }

  client.loop();

}
