#include <WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char addr[] = "192.168.76.242";
int port = 7001;

IPAddress dest;
WiFiUDP udp;

void setup() {
   Serial.begin(115200);

   // WIFI
   Serial.println("[WIFI_CONNECTION]");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("[WIFI_FAIL]");
      delay(1000);
      ESP.restart();
   }
   Serial.printf("[WIFI_SUCCESS]");
   udp.begin(WiFi.localIP(), 7000);
}

void loop() {
   // UDP CLIENT
   Serial.println("sdasd");
   udp.beginPacket("192.168.76.242", 7001);
   udp.println("test");
   udp.endPacket();
   delay(1000);
}