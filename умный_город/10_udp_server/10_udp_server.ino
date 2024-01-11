#include <WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char addr[] = "192.168.6.242";
int port = 7000;

IPAddress dest;
WiFiUDP udp;

void setup() {
   Serial.begin(115200);
   // PINS
   pinMode(26, OUTPUT);
   pinMode(25, OUTPUT);

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
   Serial.printf("[UDP_SERVER_LISTENING]: %s:%d\n", WiFi.localIP().toString().c_str(), port);
   udp.begin(7000);
}

void loop() {
   uint8_t buffer[50];
   udp.parsePacket();
   if(udp.read(buffer, 50) > 0){
      Serial.print("Server to client: ");
      Serial.println((char *)buffer);
   }
}