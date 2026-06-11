#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <PubSubClient.h>

#include "secrets.h"
#include "tv_ir_codes.h"

#define IR_SEND_PIN D1

constexpr const char *kMqttTopicCmd = "3bfab/tv/power/cmd";
constexpr const char *kMqttTopicStatus = "3bfab/tv/status";
constexpr const char *kMqttDiscovery = "homeassistant/button/3bfab_tv_power/config";

constexpr uint8_t kNecBurstCount = 3;
constexpr uint16_t kNecBurstGapMs = 110;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
IRsend irsend(IR_SEND_PIN);

const char *mqttErrorToString(int state) {
  switch (state) {
    case -4: return "Zaman asimi";
    case -3: return "Baglanti koptu";
    case -2: return "Broker'a ulasilamadi (IP/port kontrol)";
    case -1: return "Bagli degil";
    case 1: return "Protokol hatasi";
    case 2: return "Client ID reddedildi";
    case 3: return "Broker ulasilamiyor";
    case 4: return "Kullanici/sifre hatali";
    case 5: return "Yetkisiz — MQTT_USER ve MQTT_PASS gerekli";
    default: return "Bilinmeyen hata";
  }
}

void blinkStatusLed() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(80);
  digitalWrite(LED_BUILTIN, HIGH);
}

void sendTvPower() {
  Serial.println(F("[IR] TV On/Off gonderiliyor..."));

  for (uint8_t i = 0; i < kNecBurstCount; i++) {
    irsend.sendNEC(TV_POWER_CODE, TV_POWER_BITS);
    if (i + 1 < kNecBurstCount) {
      delay(kNecBurstGapMs);
    }
  }

  blinkStatusLed();
  Serial.println(F("[IR] Gonderildi"));
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  String message;
  message.reserve(length + 1);
  for (unsigned int i = 0; i < length; i++) {
    message += static_cast<char>(payload[i]);
  }

  Serial.print(F("[MQTT] "));
  Serial.print(topic);
  Serial.print(F(" -> "));
  Serial.println(message);

  if (message == F("PRESS") || message == F("ON") || message == F("TOGGLE") ||
      message == F("1")) {
    sendTvPower();
  }
}

void publishHomeAssistantDiscovery() {
  const char payload[] =
      "{"
      "\"name\":\"TV Ac/Kapa\","
      "\"unique_id\":\"3bfab_tv_power\","
      "\"command_topic\":\"3bfab/tv/power/cmd\","
      "\"payload_press\":\"PRESS\","
      "\"availability_topic\":\"3bfab/tv/status\","
      "\"payload_available\":\"online\","
      "\"payload_not_available\":\"offline\","
      "\"device\":{"
      "\"identifiers\":[\"3bfab_tv_ir\"],"
      "\"name\":\"TV IR Kontrol\","
      "\"model\":\"Wemos D1 mini\","
      "\"manufacturer\":\"3BFab\""
      "}"
      "}";

  mqtt.publish(kMqttDiscovery, payload, true);
  Serial.println(F("[MQTT] Home Assistant discovery yayinlandi"));
}

bool connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  Serial.print(F("[WiFi] Baglaniyor: "));
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.hostname(MQTT_CLIENT_ID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint8_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print('.');
    attempts++;
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("[WiFi] Baglanti basarisiz"));
    return false;
  }

  Serial.print(F("[WiFi] IP: "));
  Serial.println(WiFi.localIP());
  return true;
}

bool connectMqtt() {
  if (mqtt.connected()) {
    return true;
  }

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512);

  Serial.print(F("[MQTT] Broker: "));
  Serial.println(MQTT_BROKER);

  const bool hasMqttAuth = (MQTT_USER[0] != '\0');
  const bool connected = hasMqttAuth
      ? mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS,
                     kMqttTopicStatus, 0, true, "offline")
      : mqtt.connect(MQTT_CLIENT_ID, kMqttTopicStatus, 0, true, "offline");

  if (!connected) {
    Serial.print(F("[MQTT] Hata "));
    Serial.print(mqtt.state());
    Serial.print(F(": "));
    Serial.println(mqttErrorToString(mqtt.state()));
    if (mqtt.state() == 5) {
      Serial.println(F("[MQTT] HA Mosquitto kullanici/sifre secrets.h icine yazin"));
    }
    return false;
  }

  mqtt.publish(kMqttTopicStatus, "online", true);
  mqtt.subscribe(kMqttTopicCmd);
  publishHomeAssistantDiscovery();

  Serial.println(F("[MQTT] Baglandi"));
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  irsend.begin();

  Serial.println();
  Serial.println(F("=== 3BFab TV IR — Home Assistant ==="));
  Serial.println(F("Home Assistant'da 'TV Ac/Kapa' butonu otomatik gorunur"));
  Serial.println(F("Serial: t = manuel IR gonder"));
  Serial.println();

  connectWiFi();
  connectMqtt();
}

void loop() {
  if (Serial.available()) {
    const char cmd = static_cast<char>(Serial.read());
    if (cmd == 't' || cmd == 'T') {
      sendTvPower();
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqtt.connected()) {
    connectMqtt();
  }

  mqtt.loop();
  yield();
}
