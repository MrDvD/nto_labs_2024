// #include <Dragster .h>

// Dragster robot(MMAX_16_OHM);
int l = 0, r = 0;
int prev_l = 0, prev_r = 0;
int PIN_LEFT = 25, PIN_RIGHT = 26;

xTaskHandle task_enc, task_mot;

void setup(){
   pinMode(PIN_LEFT, INPUT);
   pinMode(PIN_RIGHT, INPUT);
   pinMode(17, OUTPUT);
   pinMode(14, OUTPUT);
   Serial.begin(115200);
   
   ledcSetup(0, 5000, 8);
   ledcSetup(1, 5000, 8);
   ledcAttachPin(27, 0);
   ledcAttachPin(16, 1);
   Serial.println("[BEGIN]");
   
   // digitalWrite(14, LOW);
   // ledcWrite(0, 255);
   // digitalWrite(17, HIGH);
   // ledcWrite(1, 255);
   // Serial.println("[MID]");
   // delay(2000);
   // ledcWrite(0, 0);
   // ledcWrite(1, 0);
   // Serial.println("[DONE]");

   
   // robot.begin();
   // robot.encodersBegin(left, right);
   // robot.drive(60, 0);
   xTaskCreatePinnedToCore(enc, "encoders", 10000, NULL, 1, &task_enc, 0);
   xTaskCreatePinnedToCore(motors, "motors", 10000, NULL, 1, &task_mot, 1);
}
void loop(){
   delay(10);
}

void motors(void* params){
   while (true){
      int razn = r - l;
      int znach = computePID(razn, 0, 3, 0, 0, 100);
      Serial.printf("Razn %d Zero 0\n", razn);
      
      
      digitalWrite(14, LOW);
      ledcWrite(0, 220 - znach);
      digitalWrite(17, HIGH);
      ledcWrite(1, 220 + znach);
      delay(100);
   }
   
}

void enc(void*params){
   while (true) {
      int curr_r = digitalRead(PIN_RIGHT), curr_l = digitalRead(PIN_LEFT);
      if (curr_r != prev_r) {
         prev_r = curr_r;
         r += 1;
      }
      if (curr_l != prev_l) {
         prev_l = curr_l;
         l += 1;
      }
      // Serial.printf("l %d r %d\n", l, r);
      delay(25);
   }
}

int computePID(float input, float setpoint, float kp, float ki, float kd, float dt) {
  float err = setpoint - input;
  static float integral = 0, prevErr = 0;
  integral += err * dt;
  float D = (err - prevErr) / dt;
  prevErr = err;
  return (err * kp + integral * ki + D * kd);
}