#include <WiFi.h>
#include <WiFiUdp.h>
#include <cmath>
#include <bitset>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char addr[] = "192.168.52.242";
int port = 7001;

WiFiUDP udp;

int D0 = 14;
int D1 = 27;
int RESET = 17;
int DC = 12;
int CS = 13;

Adafruit_SSD1306 display(D1, D0, DC, RESET, CS);

std::string pack;
int user = 0, charge = 0, mark1 = 0, mark2 = 0, charge2 =0;
int val = -1;
bool wasUp1 = false, wasUp2 = false, isUp = false, wasUp = false;

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
      Serial.printf("\r");
      display.display();
      
      if ((wasUp == false) && (isUp == true)){
         mark1 = millis();
         mark2 = -1;
      }else if ((wasUp == true) && (isUp == false)){
         mark2 = millis();
         mark1 = -1;
      }
      Serial.printf("%d %d %d %d\n", mark1, mark2, isUp, wasUp);
      charge2 = charge;
      int curr = millis();
      if (mark2 == -1 && isUp && curr - mark1 > 150){
            charge2 += 1;
            mark1 = curr;
            val = 1;
      }else if (mark1 == -1 && !isUp && curr - mark2 > 150) {
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
      if (!(charge2 == charge)) {
         
         pack = int_to_binary(charge2, 1);
         udp.beginPacket(addr, port);
         udp.println(pack.c_str());
         udp.endPacket(); 
         // Serial.println(pack.c_str());
      }
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
      charge = charge2;
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
   // TASKS
   xTaskCreatePinnedToCore(send, "s", 10000, NULL, 1, &task_send, 1);
   xTaskCreatePinnedToCore(click, "c", 10000, NULL, 1, &task_cl, 0);
}

void loop() {
   vTaskDelay(100);
}