#include <WiFi.h>
#include <WiFiUdp.h>
#include <cmath>
#include <bitset>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char addr[] = "192.168.52.242";
int port = 7001;

WiFiUDP udp;

std::string pack;
int clicks = 0, user = 0;
bool wasUp1 = false, wasUp2 = false;

TaskHandle_t task_cl, task_send;

std::string int_to_binary(int num, bool sign) {
   std::string binarized;
   if (sign) {
      num = abs(num);
      binarized += num >= 0 ? '0' : '1';
   }
   // change '8' to desired bit count
   binarized += std::bitset<7>(num).to_string();
   return binarized;
}

void send(void *params){
   while (true) {
      bool isUp2 = digitalRead(26);
      Serial.printf("\r");
      if (!wasUp2 && isUp2) {
         pack = int_to_binary(user, 1) + int_to_binary(clicks, 1);
         udp.beginPacket(addr, port);
         udp.println(pack.c_str());
         udp.endPacket();
         Serial.println(pack.c_str());
         // reset
         user++;
         user %= 4;
         clicks = 0;
      }
      wasUp2 = isUp2;
   }
}

void click(void *params){
   while (true){
      bool isUp1 = digitalRead(25);
      if (wasUp1 && !isUp1) {
         clicks += 1;
      }
      wasUp1 = isUp1;
   }
}

void setup() {
   Serial.begin(115200);
   // PINS
   pinMode(26, INPUT_PULLDOWN);
   pinMode(25, INPUT);
   // WIFI
   Serial.println("WIFI_CONNECTION");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   if (WiFi.waitForConnectResult() != WL_CONNECTED){
      Serial.println("Fail");
      delay(1000);
      ESP.restart();
   }
   Serial.printf("Success");
   udp.begin(WiFi.localIP(), 7000);
   // TASKS
   xTaskCreatePinnedToCore(send, "s", 10000, NULL, 1, &task_send, 0);
   xTaskCreatePinnedToCore(click, "c", 10000, NULL, 1, &task_cl, 1);
}

void loop() {
   vTaskDelay(100);
}