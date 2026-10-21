#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "";
const char* password = "";

// Free public broker (accessible without an account)
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Unique topics (use unique prefixes to avoid interference)
const char* topic_sub = "ashish_lab/led";
const char* topic_pub = "ashish_lab/status";

const int LED_PIN = 4;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Incoming on [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (msg == "1") {
    digitalWrite(LED_PIN, HIGH);
    client.publish(topic_pub, "LED is ON");
  } else if (msg == "0") {
    digitalWrite(LED_PIN, LOW);
    client.publish(topic_pub, "LED is OFF");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    String clientId = "ESP32S3-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println(" Connected!");
      client.subscribe(topic_sub);
      client.publish(topic_pub, "ESP32-S3 Online");
    } else {
      Serial.print(" Failed (rc=");
      Serial.print(client.state());
      Serial.println("). Retrying in 4s...");
      delay(4000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected.");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}