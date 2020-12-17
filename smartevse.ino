#include <ModbusMaster.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define MAX485_RE_NEG  4

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_SERVER_IPADDRESS";
WiFiClient espClient;
PubSubClient client(espClient);

ModbusMaster node;

boolean startCharging = false;
boolean stopCharging = false;
int chargeCurrent = 6;

unsigned long lastMillis = 0;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
}

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 0);
  Serial2.begin(9600);
  node.begin(1, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.begin(115200);
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

  if (String(topic) == "smartevse/current") {
    current_to_smartevse(messageTemp.toInt());
  }

  if (String(topic) == "smartevse/start") {
    startCharging = true;
  }

  if (String(topic) == "smartevse/stop") {
     stopCharging = true;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("smartevseclient")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("smartevse/current");
      client.subscribe("smartevse/start");
      client.subscribe("smartevse/stop");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void current_to_smartevse(int current) {
  Serial.print("Changing current to ");
  Serial.println(current);
  if (client.connected()) {
    client.publish("smartevse/debug", "Changing current");
  }

  chargeCurrent = current;
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  if (millis() - lastMillis > 1000)
  {
    node.writeSingleRegister(0x00A6, chargeCurrent * 10);
    Serial.print("Writing current to smartEVSE, current: ");
    Serial.println(chargeCurrent);
    lastMillis = millis();
  }

  if (startCharging == true)
  {
    Serial.println("Start charging");

    if (client.connected()) {
      client.publish("smartevse/debug", "Start charging");
    }
    node.writeSingleRegister(0x00A7, 1);
    startCharging = false;
  }

  if (stopCharging == true) {
    Serial.println("Stop charging");
    if (client.connected()) {
      client.publish("smartevse/debug", "Stop charging");
    }
    node.writeSingleRegister(0x00A7, 0);
    stopCharging = false;
  }

  client.loop();
}
