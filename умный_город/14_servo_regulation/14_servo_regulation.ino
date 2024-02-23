#include <WiFi.h>
#include <WiFiUdp.h>
#include <cmath>
#include <bitset>
#include <ESP32_Servo.h>

const char ssid[] = "Testy";
const char pass[] = "22222222";

const char addr[] = "server.tasty";
int port = 7001;

Servo myservo;

WiFiUDP udp;
WiFiUDP udp1;

std::string pack;

TaskHandle_t task_read, task_send, task_get_angle;

int x_val, y_val, angle, angle_prev, angle_p2, change = 1, t1=0;

std::string int_to_binary(int num, bool sign) {
   std::string binarized;
   if (sign) {
      num = abs(num);
      binarized += num >= 0 ? '0' : '1';
   }
   // change '7' to desired bit count
   binarized += std::bitset<9>(num).to_string();
   return binarized;
}

int binary_to_int(const char *str, bool sign) {
    std::string binary = std::string(str);
    int result;
    if (sign) {
        int sign = binary[0] - '0';
        result = std::strtol(binary.substr(1).c_str(), 0, 2);
        if (sign > 0) {
            result *= -1;
        }
    } else {
        result = std::strtol(binary.c_str(), 0, 2);
    }
    return result;
}

void read_joy(void *params){
   while (true){
      x_val = analogRead(34) ;
      y_val = analogRead(35) ;
      delay(10);
      vTaskDelay(0);
   }
}

void set_servo(){
   angle_p2 = angle_prev;
   if (angle_p2 > angle){
      for (int j=angle_p2; j> angle; j--){
         myservo.write(j);
         delay(2);
      }
      
   } else if (angle_p2 < angle) {
      for (int j=angle_p2; j < angle; j++){
         myservo.write(j);
         delay(2);
      } 
   }
   angle_p2 = angle;
}


void get_angle(void *params){
   while (true){
      
      udp1.beginPacket(addr, port);
      udp1.println("1");
      udp1.endPacket();
      while (true){
         if (millis() - t1 > 5000){
            change = 1;
         }
         udp1.parsePacket();
         uint8_t buffer_i[20];
         if (udp1.read(buffer_i, 20)>0){
            udp1.read(buffer_i, 20);
            angle_prev = angle;
            angle = binary_to_int((char *)buffer_i, 0);
            Serial.printf("Got angle: %d buffer: %s\n", angle, buffer_i);
            set_servo();
            change = 0;
            t1 = millis();
            break;
         }
      }
      vTaskDelay(0);     
   }
}

void send_angle(void *params){
   while (true){
      if ((y_val == 4095 || x_val == 0 || x_val == 4095) && change){
         if (y_val- 1901 > 1900) {
            y_val = 4095;
         }
         int angle2 = (std::atan2(y_val- 1901, x_val- 1956) * 180 / 3.1416);
         // Serial.printf("Angle %d x %d y %d\n", angle2, x_val, y_val);
         
         if (angle2 != angle && angle2 >= 0) {
            set_servo();
            pack = int_to_binary(angle2, 0);
            udp.beginPacket(addr, port);
            udp.println(pack.c_str());
            udp.endPacket(); 
            // Serial.println(pack.c_str());
            angle_prev = angle;
            angle = angle2;
         }
      } 
      vTaskDelay(0);  
   }
}

void setup() {
   Serial.begin(115200);
   
   Serial.println("Hello");
   
   myservo.attach(26);
   
   // WIFI
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
   udp1.begin(7002);
   
   xTaskCreatePinnedToCore(read_joy, "read", 10000, NULL, 1, &task_read, 0);
   xTaskCreatePinnedToCore(get_angle, "get", 10000, NULL, 1, &task_get_angle, 1);
   xTaskCreatePinnedToCore(send_angle, "send", 10000, NULL, 1, &task_send, 1);
}

void loop() {
   vTaskDelay(0); 
}