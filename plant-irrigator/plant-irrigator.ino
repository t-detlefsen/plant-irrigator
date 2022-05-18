/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "SSID_NAME";
const char* password = "PASSWORD";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.1.87";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int msgID = 0;
char msgString[20];

//uncomment the following lines if you're using SPI
/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

Adafruit_AHTX0 aht; // I2C
const int moistPin = 34;
const int AirValue = 305;
const int WaterValue = 145;

float temperature = 0;
float humidity = 0;
float moistureRes = 0;
float moisturePrev = 0;
float moisture = 0;

bool flagUp = false;

void setup() {
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  //status = bme.begin();  
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1200000) {
    lastMsg = now;
    
    // Temperature in Celsius
    sensors_event_t humid, temp;
    aht.getEvent(&humid, &temp);   
    temperature = 1.8 * temp.temperature + 32; // Temperature in Fahrenheit

    // Convert the value to a char array
    sprintf(msgString, "%d", msgID);
    
    // Convert the value to a char array
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);

    humidity = humid.relative_humidity;
    
    // Convert the value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);

    moisture = 0;
    char moistString3[8];
    for (int i = 0; i < 100; i++) {
      moisture += analogRead(moistPin);
      delay(2);
    }

    moisture /= 100;

    moisture = map(moisture, AirValue, WaterValue, 0, 100);

    // Convert the value to a char array
    char moistString[8];
    dtostrf(moisture, 1, 2, moistString);
    char moistString2[8];
    dtostrf(moisturePrev, 1, 2, moistString2);
    Serial.print("Moisture: ");
    Serial.println(moistString);
    Serial.println(moistString2);

    char payload[100] = "{\n";
    strcat(payload, "\t\"_msgid\": ");
    strcat(payload, msgString);
    strcat(payload, ",\n\t\"temperature\": ");
    strcat(payload, tempString);
    strcat(payload, ",\n\t\"humidity\": ");
    strcat(payload, humString);
    strcat(payload, "\n}");

    client.publish("window_sill", payload);

    char plantload[100] = "{\n";
    strcat(plantload, "\t\"_msgid\": ");
    strcat(plantload, msgString);
    strcat(plantload, ",\n\t\"moisture\": ");
    strcat(plantload, moistString);
    strcat(plantload, "\n}");

    client.publish("basil", plantload);

    // Increment Message ID
    msgID++;
  }
}
