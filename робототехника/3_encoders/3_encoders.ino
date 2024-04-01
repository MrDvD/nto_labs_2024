#include <WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "Testy";
const char pass[] = "22222222";
const char addr[] = "server.tasty";
int port = 7001;

int l = 0, r = 0;
int prev_l = 0, prev_r = 0;
int PIN_LEFT = 25, PIN_RIGHT = 26;
int k_p = 8;

WiFiUDP udp, udp1;
xTaskHandle task_enc, task_sync, task_recv;

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
   udp.begin(WiFi.localIP(), 7000);
   udp1.begin(7002);
   // TASKS
   xTaskCreatePinnedToCore(encoders, "encoders", 10000, NULL, 1, &task_enc, 0);
   xTaskCreatePinnedToCore(sync, "sync", 10000, NULL, 1, &task_sync, 1);
   xTaskCreatePinnedToCore(recv_params, "recv_params", 10000, NULL, 1, &task_recv, 1);
   // MOVEMENT
   drive(255, 255);
}

void loop(){
   delay(10);
}

void encoders(void*params){
   // count encoders
   while (true){
      int curr_r = digitalRead(PIN_RIGHT), curr_l = digitalRead(PIN_LEFT);
      if (curr_r != prev_r) {
         prev_r = curr_r;
         r += 1;
      }
      if (curr_l != prev_l) {
         prev_l = curr_l;
         l += 1;
      }
      delay(20);
   }
}

void send_logs(int P) {
   // send logs to server
   udp.beginPacket(addr, port);
   std::string pack = "l: ";
   pack += std::to_string(150 + P);
   pack += " r: ";
   pack += std::to_string(150 - P);
   pack += " diff (2 * P): ";
   pack += std::to_string(2 * P);
   
   udp.println(pack.c_str());
   udp.endPacket();
}

void recv_params(void* params) {
   // receive params for PID
   while (true) {
      udp1.parsePacket();
      uint8_t buffer[20];
      if (udp1.read(buffer, 20) > 0) {
         // Serial.println((char *) buffer);
         k_p = std::stoi(std::string((char *) buffer));
      }
      delay(50);
      Serial.printf("k_p %d\n", k_p); 
   }
}

void sync(void* params){
   // synchronization of motors
   while (true){
      int prev_l = l, prev_r = r;
      delay(150);
      prev_l = l - prev_l;
      prev_r = r - prev_r;
      // int pid = computePID(prev_r-prev_l, 0, 3, 0, 0, 100);
      int err = prev_r - prev_l;
      int P = k_p * err;
      // Serial.printf("l %d r %d prev_l %d prev_r %d\n", 150+pid, 150-pid, prev_l, prev_r);
      drive(150 + P, 150 - P);
      send_logs(P);
   }
}

// int computePID(float input, float setpoint, float kp, float ki, float kd, float dt) {
//    float err = setpoint - input;
//    static float integral = 0, prevErr = 0;
//    integral += err * dt;
//    float D = (err - prevErr) / dt;
//    prevErr = err;
//    return (err * kp + integral * ki + D * kd);
// }