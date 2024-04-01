#include <WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "Testy";
const char pass[] = "22222222";

const char addr[] = "server.tasty";
int port = 7001;

WiFiUDP udp;

const int trigPin = 16;
const int echoPin = 17;
int distanceCm;

int res_prev = 0;
bool mark;

xTaskHandle task_mot;

void setup(){
   Serial.begin(115200);

   pinMode(25, OUTPUT);
   pinMode(26, OUTPUT);
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   
   Serial.println("WIFI_CONNECTION......");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   if (WiFi.waitForConnectResult() != WL_CONNECTED){
      Serial.println("Fail");
      delay(1000);
      ESP.restart();
   }
   Serial.printf("Success\n");
   udp.begin(WiFi.localIP(), 7000);
   
   // xTaskCreatePinnedToCore(distance, "irrecv", 10000, NULL, 10, &task_ir, 0);
   xTaskCreatePinnedToCore(distance, "move", 10000, NULL, 10, &task_mot, 1);
   delay(1000);
   smove();
}
void loop(){
   delay(10);
}

void distance(void* params){
   while (true){
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      
      // Reads the echoPin, returns the sound wave travel time in microseconds
      long duration = pulseIn(echoPin, HIGH);
      
      // Calculate the distance
      distanceCm = duration * 0.034/2;
      delay(100);
   }
}

void smove(){
   digitalWrite(26, HIGH);
   digitalWrite(25, HIGH);
   int t1 = millis();
   int t2 = t1;
   int angle = 0;
   while (millis() - t1 < 1000){
      if (millis() - t2 > 9){
         t2 = millis();
         angle += 5;
      }
      if (angle > 181){
         break;
      }
      std::string pack = std::to_string(angle);
      pack += ' ';
      pack += std::to_string(distanceCm);
      udp.beginPacket(addr, port);
      udp.println(pack.c_str());
      udp.endPacket(); 
   }
   digitalWrite(26, LOW);
   digitalWrite(25, LOW);
}
void interp(void *params){
   while (true){
      Serial.printf("res %d time %d\n", res_prev, (int) millis());
      switch (res_prev) {
         case 16718055:
            while (mark) {
               ledcWrite  (1, 255);
               ledcWrite  (0, 255);
               delay(20);
            }
            break;
         case 16730805:
            while (mark) {
               ledcWrite  (1, 0);
               ledcWrite  (0, 0);
               delay(20);
            }
            break;
         case 16734885:
            while (mark) {
               ledcWrite  (1, 0);
               ledcWrite  (0, 255);
               delay(20);
            }
            break;
         case 16716015:
            while (mark) {
               ledcWrite  (1, 255);
               ledcWrite  (0, 0);
               delay(20);
            }
            break;
      } 
      ledcWrite(1, 0);
      ledcWrite(0, 0);
      delay(50);
   }
}

// void motors(int left, int right){
//    if (left){}
// }