# include <WiFi.h>

const char ssid[]= "Redmi Note 10S";
const char pass[] = "11111111";

const char ip[] = "192.168.155.242";
const uint16_t port = 7000;

void setup() {
   Serial.begin(115200);
   
   // PINS
   pinMode(12, INPUT);
   
   // WI-FI CONNECTION
   Serial.println("[WIFI_CONNECTION]");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   if (WiFi.waitForConnectResult()!= WL_CONNECTED) {
      Serial.println("[WIFI_FAIL]");
      delay(1000);
      ESP.restart();
   }
   Serial.printf("[WIFI_SUCCESS]");
}

void loop() {
   // SERVER CONNECTION
   WiFiClient client;
   Serial.printf("[SERVER_CONNECTION]: %s\n", ip);
   if (!client.connect(ip, port)){
      Serial.println("[SERVER_FAIL]");
      delay(2500);
      return;
   }
   Serial.println("[SERVER_SUCCESS]");
   
   // SERVER INTERACTION
   while(client.connect(ip, port)){
      bool value = digitalRead(12);
      Serial.printf("[BUTTON]: %d\n", value);
      client.printf("%d", value);
      delay(250);
   }
   client.stop();
   Serial.println("[SERVER_DISCONNECT]");
   delay(500);
}
