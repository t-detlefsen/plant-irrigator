/* @author: Thomas Detlefsen
 * @info: Publish temperature, humidity, and 
 *        soil moisture to an MQTT broker
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

const char* ssid = "SSID_NAME";
const char* password = "PASSWORD";
const char* mqtt_server = "MQTT_ADDRESS";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int msgID = 0;
char msgString[20];
const int updateTime = 1200;

Adafruit_AHTX0 aht;
const int moisturePin = 34;
const int AirValue = 3550;
const int WaterValue = 1600;

float temperature = 0;
float humidity = 0;
int moistureRes = 0;
float moisture = 0;

const int buttonPin = 4;
const int pumpPin =  5;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  } Serial.println("AHT10 or AHT20 found");
  
  setup_wifi();
  client.setCallback(callback);
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("basil/water");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
 
  for (int i = 0; i < length; i++) {
//    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  int waterTime = messageTemp.toInt();
  Serial.print(waterTime);
  Serial.println();

  Serial.print("Changing output to ");
  Serial.println("on");
  digitalWrite(pumpPin, HIGH);
  delay(waterTime * 1000);
  Serial.println("off");
  digitalWrite(pumpPin, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > updateTime * 1000) {
    lastMsg = now;
    
    // Get sensor values
    sensors_event_t humid, temp;
    aht.getEvent(&humid, &temp);
    temperature = 1.8 * temp.temperature + 32; // Temperature in Fahrenheit

    humidity = humid.relative_humidity;

    moistureRes = analogRead(moisturePin);
    moisture = map(moistureRes, AirValue, WaterValue, 0, 100);

    // Print sensor values to serial
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);

    char moistString[8];
    dtostrf(moisture, 1, 2, moistString);
    Serial.print("Moisture: ");
    Serial.println(moistString);
    
    sprintf(msgString, "%d", msgID);

    // Format values as JSON for MQTT transfer
    char payload[100] = "{\n";
    strcat(payload, "\t\"_msgid\": ");
    strcat(payload, msgString);
    strcat(payload, ",\n\t\"temperature\": ");
    strcat(payload, tempString);
    strcat(payload, ",\n\t\"humidity\": ");
    strcat(payload, humString);
    strcat(payload, "\n}");

    char plantload[100] = "{\n";
    strcat(plantload, "\t\"_msgid\": ");
    strcat(plantload, msgString);
    strcat(plantload, ",\n\t\"moisture\": ");
    strcat(plantload, moistString);
    strcat(plantload, "\n}");

    // Publish message
    client.publish("window_sill", payload);
    client.publish("basil", plantload);

    // Increment Message ID
    msgID++;
  }
}
