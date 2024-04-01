#define IR_RECEIVE_PIN 34
# include <IRremote.hpp>

// IRrecv irrecv(34);
// decode_results results;

int l=0, r=0;
char dir = 's';

#define DECODE_NEC

xTaskHandle task_mot;
void recieveCb(){
   IrReceiver.decode();
   Serial.println(IrReceiver.decodedIRData.command, HEX);
   if (IrReceiver.decodedIRData.command == 0x18){
      //forward
      dir = 'f';
   }else if (IrReceiver.decodedIRData.command == 0x5A){
      //right
      dir = 'r';
   }else if (IrReceiver.decodedIRData.command == 0x52){
      //backward
      dir = 'b';
   }else if (IrReceiver.decodedIRData.command == 0x8){
      //left
      dir = 'l';
   }else if (IrReceiver.decodedIRData.command == 0x1C){
      //stop
      dir = 's';
   }
   IrReceiver.resume();
}
void setup(){
   Serial.begin(115200);
   pinMode(34, INPUT);
   init_drive(27, 16, 14, 17);
   IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
   IrReceiver.registerReceiveCompleteCallback(recieveCb);
   // time = timerBegin(1, 80, 1);
   xTaskCreatePinnedToCore(interp, "move", 10000, NULL, 10, &task_mot, 1);
}
void loop(){
   delay(10);
}

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

void interp(void *params){
   while (true){
      switch (dir) {
         case 'f':
            drive(150,150);
            break;
         case 'b':
            drive(-150, -150);
            break;
         case 'l':
            drive(100, 190);
            break;
         case 'r':
           drive(190, 100);
           break; 
         case 's':
            drive(-0, 0);
            break;
      }
      delay(50);
   }
}