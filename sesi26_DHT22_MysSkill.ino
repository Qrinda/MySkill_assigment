#include <WiFi.h>             
#include <PubSubClient.h>
#include <DHT.h>

// Informasi jaringan WiFi
const char* ssid = "Q";
const char* password = "qriara2843";

// Informasi MQTT broker
const char* mqtt_server = "192.168.100.5"; 
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN 4           
#define DHTTYPE DHT22      
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  dht.begin();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Mencoba terhubung ke MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("terhubung");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" mencoba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca data sensor!");
    return;
  }

  String suhu = "Suhu: " + String(t) + " °C";
  String kelembaban = "Kelembaban: " + String(h) + " %";
  Serial.println(suhu);
  Serial.println(kelembaban);

  // Kirim data ke topik MQTT
  client.publish("/sensor/suhu", String(t).c_str());
  client.publish("/sensor/kelembaban", String(h).c_str());

  delay(20000); 
}
