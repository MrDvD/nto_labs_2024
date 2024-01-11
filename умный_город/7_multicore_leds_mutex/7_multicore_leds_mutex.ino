#include <WiFi.h>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char ip[] = "192.168.6.242";
const uint16_t port = 7000;

int state[] = {-1, -1};

// TASKS
TaskHandle_t task_l2, task_hi, task_l1;

// MUTEXES
SemaphoreHandle_t mtx1 = xSemaphoreCreateMutex();
SemaphoreHandle_t mtx2 = xSemaphoreCreateMutex();

void blink_led_1(void *params){
   while (true) {
      digitalWrite(26, HIGH);
      xSemaphoreTake(mtx1, portMAX_DELAY);
      state[0] = 1;
      xSemaphoreGive(mtx1);
      delay(250);
      digitalWrite(26, LOW);
      xSemaphoreTake(mtx1, portMAX_DELAY);
      state[0] = 0;
      xSemaphoreGive(mtx1);
      delay(250);
   }  
}

void blink_led_2(void *params){
   while (true) {
      digitalWrite(25, HIGH);
      xSemaphoreTake(mtx2, portMAX_DELAY);
      state[1] = 1;
      xSemaphoreGive(mtx2);
      delay(500);
      digitalWrite(25, LOW);
      xSemaphoreTake(mtx2, portMAX_DELAY);
      state[1] = 0;
      xSemaphoreGive(mtx2);
      delay(500);
   }  
}

void tcp_interaction(void *params){
   Serial.println("[WIFI_CONNECTION]");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("[WIFI_FAIL]");
      delay(1000);
      ESP.restart();
   }
   Serial.printf("[WIFI_SUCCESS]");
   while (true) {
      WiFiClient client;
      Serial.printf("[SERVER_CONNECTION]: %s\n", ip);
      if (!client.connect(ip, port)){
         Serial.println("[SERVER_FAIL]");
         delay(2500);
         continue;
      }
      Serial.println("[SERVER_SUCCESS]");
      
      // SERVER INTERACTION
      while (client.connect(ip, port)) {
         xSemaphoreTake(mtx1, portMAX_DELAY);
         xSemaphoreTake(mtx2, portMAX_DELAY);
         client.printf("%d %d", state[0], state[1]);
         xSemaphoreGive(mtx1);
         xSemaphoreGive(mtx2);
         String ans = client.readStringUntil('\n');
         Serial.println(ans);
         delay(250);
      }
      client.stop();
      Serial.println("[SERVER_DISCONNECT]");
      delay(2500);
   }
}

void setup() {
   Serial.begin(115200);
   // PINS
   pinMode(26, OUTPUT);
   pinMode(25, OUTPUT);
   // TASKS
   xTaskCreatePinnedToCore(blink_led_1, "l1", 10000, NULL, 1, &task_l1, 0);
   xTaskCreatePinnedToCore(blink_led_2, "l2", 10000, NULL, 1, &task_l2, 0);
   xTaskCreatePinnedToCore(tcp_interaction, "ti", 10000, NULL, 1, &task_hi, 1);
}

void loop() {
   // DO NOT CODE HERE
   vTaskDelay(100);
}