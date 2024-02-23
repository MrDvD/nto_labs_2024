#include <WiFi.h>
#include <WiFiUdp.h>
#include <cmath>
#include <bitset>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char ssid[] = "Testy";
const char pass[] = "22222222";

const char addr[] = "server.tasty";
int port = 7001;

WiFiUDP udp;
WiFiUDP udp1;

int D0 = 27;
int D1 = 14;
int RESET = 17;
int DC = 12;
int CS = 13;

Adafruit_SSD1306 display(D1, D0, DC, RESET, CS);

std::string pack;
int user = 0, charge = 0, mark1 = 0, mark2 = 0, charge2 =0, isWaiting = 0;
int val = -1;
bool wasUp1 = false, wasUp2 = false, isUp = false, wasUp = false;

TaskHandle_t task_cl, task_send, task_get, task_refresh;

std::string int_to_binary(int num, bool sign) {
   std::string binarized;
   if (sign) {
      num = abs(num);
      binarized += num >= 0 ? '0' : '1';
   }
   // change '7' to desired bit count
   binarized += std::bitset<8>(num).to_string();
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

void start_waiting(void*params){
   while (true){
      udp1.beginPacket(addr, port);
      udp1.println("1");
      udp1.endPacket();
      while (true){
         udp1.parsePacket();
         uint8_t buffer_i[20];
         if (udp1.read(buffer_i, 20)>0){
            user = binary_to_int((char *)buffer_i, 0);
            Serial.println(user);
            break;
         }
      }
      vTaskDelay(0);     
   }
}

void display_refresh(void *params){
   while(true){
      display.display();
      display.clearDisplay();
      display.drawRect(5, 27, 93, 12, 1);
      for (int i=0; i <= (int) (charge2/10) - 1; i++){
         display.fillRect(7 + i*9, 29, 8, 8, 1);
      } 
       
      display.setCursor(10,0);
      display.setTextSize(1);
      display.setTextColor(1);
      display.printf("Charge Station");
      
      
      display.setCursor(10,50);
      display.setTextSize(1);
      display.setTextColor(1);
      
      switch (val) {
         case 0: display.printf("Discharged");
            break;
         case 2: display.printf("Discharging");
            break;
         case 1: display.printf("Charging");
            break;
         case 3: display.printf("Charged");
            break;
      }
      
      display.setCursor(100,30);
      display.setTextSize(1);
      display.setTextColor(1);
      display.printf("%d%%", charge2);
      display.display();
      vTaskDelay(10);
   }  
}

void send(void *params){
   while (true) {
      Serial.printf("\r");
      
      
      if ((wasUp == false) && (isUp == true)){
         mark1 = millis();
         mark2 = -1;
      }else if ((wasUp == true) && (isUp == false)){
         mark2 = millis();
         mark1 = -1;
      }
      // Serial.printf("%d %d %d %d\n", mark1, mark2, isUp, wasUp);
      charge2 = charge;
      int curr = millis();
      if (mark2 == -1 && isUp && curr - mark1 > (1000 + user*250)){
            charge2 += 1;
            mark1 = curr;
            val = 1;
      }else if (mark1 == -1 && !isUp && curr - mark2 > (1000 - user*250)) {
         charge2 -= 1;
         mark2 = curr;
         val = 2;
      }
      if (charge2 > 100){
         charge2 = 100;
         val = 3;
      }else if (charge2 < 0) {
         charge2 = 0;
         val = 0;
      } 
      if (charge2 == 0) {
         val = 0;
      }else if (charge2 == 100) {
         val = 3;
      }
      if (!(charge2 == charge)) {
         
         pack = int_to_binary(charge2, 0) + int_to_binary(val, 0) + int_to_binary(user, 0);
         udp.beginPacket(addr, port);
         udp.println(pack.c_str());
         udp.endPacket(); 
         // Serial.println(pack.c_str());
      }
      charge = charge2;
      vTaskDelay(10);
   }
}

void click(void *params){
   while (true){
      wasUp = isUp;
      isUp = digitalRead(25);
      Serial.printf("\r");
      delay (100);
   }
}

void setup() {
   Serial.begin(115200);
   
   Serial.println("fff");
   
   display.begin(SSD1306_SWITCHCAPVCC);
   display.display();
   delay(2000);
   display.clearDisplay();

   Serial.println("22222");
   // PINS
   display.setTextSize(2);             // Normal 1:1 pixel scale
   display.setTextColor(SSD1306_WHITE);
   display.setCursor(6, 8);
   display.print("Hello");
   display.display();
   delay(1000);
   display.clearDisplay();
   display.display();
   
   pinMode(26, INPUT);
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
   udp1.begin(7002);
   // TASKS
   xTaskCreatePinnedToCore(send, "send", 10000, NULL, 1, &task_send, 1);
   xTaskCreatePinnedToCore(start_waiting, "get", 10000, NULL, 1, &task_get, 1);
   xTaskCreatePinnedToCore(display_refresh, "refresh", 10000, NULL, 1, &task_refresh, 0);
   xTaskCreatePinnedToCore(click, "click", 10000, NULL, 1, &task_cl, 0);
}

void loop() {
   vTaskDelay(100);
}