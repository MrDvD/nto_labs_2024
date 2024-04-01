#include <WiFi.h>
#include <WiFiUdp.h>

int l = 0, r = 0;
int PIN_LEFT = 25, PIN_RIGHT = 26;
int speed_l = 0, speed_r = 0;
int prev_e_l = 0, prev_e_r = 0;
float C = 13.1, L = 8.225, pi = 3.1415926, angle = 0;
char CMD = ' ';

WiFiUDP udp, udp1;
xTaskHandle task_recv, task_enc, task_ang, task_sync;

const char ssid[] = "Testy";
const char pass[] = "22222222";
const char addr[] = "server.tasty";
int port = 7001;
int sidel = 1, sider = 1;

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
   if (left > 0){
      sidel = 1;
   }else {
      sidel = -1;  
   }
   digitalWrite(14, (int) (left < 0));
   ledcWrite(0, abs(left));
   // RIGHT MOTOR
   if (right > 0){
      sider = 1;
   } else{
      sider = -1;
   }
   digitalWrite(17, (int) (right > 0));
   ledcWrite(1, abs(right));
   // Serial.printf("l %d r %d sidel %d sider %d\n", l, r, sidel, sider);
}

// void drive(int left, int right){
//    // LEFT MOTOR
//    if (left < 0){
//       digitalWrite(14, HIGH);
//       sidel = -1;
//    }else {
//       digitalWrite(14, LOW);
//       sidel = 1;  
//    }
//    ledcWrite(0, abs(left));d

//    // RIGHT MOTOR
//    if  (right > 0){
//       digitalWrite(17, HIGH);
//       sidel = 1;
//    }else{
//       digitalWrite(17, LOW);
//       sidel = -1;
//    }
//    ledcWrite(1, abs(right));
// }

void encoders(void *params){
   while (true){
      int curr_r = digitalRead(PIN_RIGHT), curr_l = digitalRead(PIN_LEFT);
      if (curr_r != prev_e_r) {
         prev_e_r = curr_r;
         r += sider;
      }
      if (curr_l != prev_e_l) {
         prev_e_l = curr_l;
         l += sidel;
      }
      delay(20);
   }
}

void anglet(void* params){
   while (true) {
      int prev_l = l, prev_r = r;
      delay(140);
      float dr = r - prev_r, dl = l - prev_l;
      float Sr = dr / 3 * C, Sl = dl / 3 * C;
      angle += (Sr - Sl)/(2*L);
      // Serial.printf("angle %s dr %d dl %d Sr-Sl %s\n", std::to_string(angle).c_str(), (int) dr, (int) dl, std::to_string(Sr - Sl).c_str());
      if (angle >= 2 * pi) {
         angle -= 2 * pi;
      } else if (angle <= -2 * pi) {
         angle += 2 * pi;
      }
      std::string packet = "";
      packet += std::to_string(r) + " " + std::to_string(l) + " " + std::to_string(dr - dr);
      udp1.beginPacket(addr, port);
      udp1.println(packet.c_str());
      udp1.endPacket();
   }
}

void sync(void *params) {
   while (true) {
      int prev_l = l, prev_r = r;
      delay(150);
      prev_l = l - prev_l;
      prev_r = r - prev_r;
      int err = prev_r - prev_l;
      
      switch (CMD) {
         case 'R':
            err = 2 - err; // [0; 2]
            break;
         case 'L':
            err = err - 2; // [-2; 0]
            break;
         case ' ':
            err = 0;
      }
      int P = 100 * err;
      if (P > 255) {
         P = 255;
      } else if (P < -255) {
         P = -255;
      }
      drive(P, -P);
   }
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
               // case 'F':
               //    Serial.println("[match_f]");
               //    speed_l +=160;
               //    speed_r +=160;
               //    break;
               case 'R':
                  Serial.println("[match_r]");
                  CMD = 'R';
                  break;
               case 'L':
                  Serial.println("[match_l]");
                  CMD = 'L';
                  break;
               // case 'B':
               //    Serial.println("[match_b]");
               //    speed_l -=160;
               //    speed_r -=160;
               //    break;
               case 'E':
                  Serial.println("[match_e]");
                  CMD = ' ';
                  break;
               case 'S':
                  Serial.println("[match_s]");
                  CMD = ' ';
                  break;
            }
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
   udp1.begin(WiFi.localIP(), 7000);
   // TASKS
   xTaskCreatePinnedToCore(listen, "receive", 10000, NULL, 1, &task_recv, 0);
   xTaskCreatePinnedToCore(encoders, "encode", 10000, NULL, 1, &task_enc, 1);
   xTaskCreatePinnedToCore(anglet, "angle", 10000, NULL, 1, &task_ang, 1);
   xTaskCreatePinnedToCore(sync, "sync", 10000, NULL, 1, &task_sync, 0);
}

void loop(){
   delay(10);
}

int computePID(float input, float setpoint, float kp, float ki, float kd, float dt) {
  float err = setpoint - input;
  static float integral = 0, prevErr = 0;
  integral += err * dt;
  float D = (err - prevErr) / dt;
  prevErr = err;
  return (err * kp + integral * ki + D * kd);
}