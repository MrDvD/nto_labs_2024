#include <WiFi.h>
#include <vector>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char ip[] = "192.168.119.242";
const uint16_t port = 7000;

std::vector<int> pins = {27, 25, 26};

void setup() {
   Serial.begin(115200);
   
   // PINS
   for (int i = 0; i < 3; i++) {
      pinMode(pins[i], OUTPUT);
   }
   
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
      client.printf("1");
      String response = client.readStringUntil('\n');
      Serial.printf("[LED]: %s\n", response.c_str());
      for (int i = 0; i < 3; i++) {
         // цикл переработан, но не проверялся на esp32
         // в случае ошибки переписать обратно в if-конструкцию
         digitalWrite(pins[i], response[i] - '0');
      }
      delay(750);      
   }
   client.stop();
   Serial.println("[SERVER_DISCONNECT]");
   delay(2500);
}
