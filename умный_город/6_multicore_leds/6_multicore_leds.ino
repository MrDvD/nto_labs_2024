#include <WiFi.h>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char ip[] = "192.168.6.242";
const uint16_t port = 7000;

TaskHandle_t task_h;
TaskHandle_t task_g;
TaskHandle_t task_j;

void function_26(void *params){
   while (true){
      digitalWrite(26, HIGH);
      delay(250);
      digitalWrite(26, LOW);
      delay(250);
   }  
}

void function_25(void *params){
   while (true){
      digitalWrite(25, HIGH);
      delay(500);
      digitalWrite(25, LOW);
      delay(500);
   }  
}

void function2(void *params){
   Serial.println("[WIFI_CONNECTION]");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   if (WiFi.waitForConnectResult()!= WL_CONNECTED) {
      Serial.println("[WIFI_FAIL]");
      delay(1000);
      ESP.restart();
   }
   Serial.printf("[WIFI_SUCCESS]");
   while(true){
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
      String ans = client.readStringUntil('\n');
      Serial.println(ans);
      delay(1000);
   }
   client.stop();
   Serial.println("[SERVER_DISCONNECT]");
   delay(2500);
   }
}

void setup() {
   Serial.begin(115200);
   pinMode(26, OUTPUT);
   pinMode(25, OUTPUT);
   xTaskCreatePinnedToCore(function_25, "j", 10000, NULL, 1, &task_j, 0);
   xTaskCreatePinnedToCore(function_26, "g", 10000, NULL, 1, &task_h, 0);
   xTaskCreatePinnedToCore(function2, "h", 10000, NULL, 1, &task_g, 1);
}

void loop() {
   // SERVER CONNECTION
   vTaskDelay(100);
}
