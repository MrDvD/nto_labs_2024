#include <WiFi.h>
#include <WiFiUdp.h>

int l = 0, r = 0;
int prev_l = 0, prev_r = 0;
int PIN_LEFT = 25, PIN_RIGHT = 26;
int speed_l = 0, speed_r = 0;

WiFiUDP udp;
xTaskHandle task_recv;

const char ssid[] = "Testy";
const char pass[] = "22222222";
const char addr[] = "server.tasty";
int port = 7001;

void init_drive(int PWM_LEFT, int PWM_RIGHT, int SPIN_LEFT, int SPIN_RIGHT) {
   pinMode(SPIN_RIGHT, OUTPUT);
   pinMode(SPIN_LEFT, OUTPUT);
   ledcSetup(0, 5000, 8);
   ledcSetup(1, 5000, 8);
   ledcAttachPin(PWM_LEFT, 0);
   ledcAttachPin(PWM_RIGHT, 1);
}

void drive(int left, int right){
   // LEFT MOTOR
   digitalWrite(14, (int) (left < 0));
   ledcWrite(0, abs(left));
   // RIGHT MOTOR
   digitalWrite(17, (int) (right > 0));
   ledcWrite(1, abs(right));
}

void listen(void *params){
   while (true){
      udp.parsePacket();
      uint8_t buffer_i[20];
      if (udp.read(buffer_i, 20)>0){
         udp.read(buffer_i, 20);
         Serial.println((char *) buffer_i);
         if ((std::string((char *) buffer_i).substr(0, 3)) == "MOV") {
            Serial.printf("char %d\n", buffer_i[4]);
            switch (buffer_i[4]){
               case 'F':
                  Serial.println("[match_f]");
                  speed_l +=160;
                  speed_r +=160;
                  break;
               case 'R':
                  Serial.println("[match_r]");
                  speed_l +=95;
                  speed_r -=95;
                  break;
               case 'L':
                  Serial.println("[match_l]");
                  speed_l -=95;
                  speed_r +=95;
                  break;
               case 'B':
                  Serial.println("[match_b]");
                  speed_l -=160;
                  speed_r -=160;
                  break;
               case 'E':
                  Serial.println("[match_e]");
                  speed_l = 0;
                  speed_r = 0;
                  break;
            }
            drive(speed_l, speed_r);
         }
      }
      delay(50);
   }
}

void setup(){
   Serial.begin(115200);
   // PINS
   pinMode(PIN_LEFT, INPUT);
   pinMode(PIN_RIGHT, INPUT);
   // PWM
   init_drive(27, 16, 14, 17);
   // WIFI
   Serial.println("[WIFI_CONNECTION]");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   if (WiFi.waitForConnectResult() != WL_CONNECTED){
      Serial.println("[WIFI_FAIL]");
      delay(1000);
      ESP.restart();
   }
   Serial.printf("Success\nIP: %s", WiFi.localIP().toString().c_str());
   // UDP
   udp.begin(7002);
   // TASKS
   xTaskCreatePinnedToCore(listen, "recive", 10000, NULL, 1, &task_recv, 0);
}

void loop(){
   delay(10);
}