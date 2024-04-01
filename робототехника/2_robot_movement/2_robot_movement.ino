
void setup(){
   Serial.begin(115200);
   ledcSetup(0, 5000, 8);
   ledcAttachPin(26, 0);
   ledcSetup(1, 5000, 6);
   ledcAttachPin(25, 1);
   ledcWrite(1, 0);
   ledcWrite(0, 0);
   Serial.println("dfd");
   
   delay(2000);
   
   for (int i=0; i<64; i++){
      ledcWrite  (1, i);
      ledcWrite  (0, i);
      Serial.println(i);
      delay(100);
   }
   delay(1000);
   for (int i=63; i>=0; i--){
      ledcWrite  (1, i);
      ledcWrite  (0, i);
      Serial.println(i);
      delay(100);
   }
   
//          ledcWrite(1, 0);
//  ledcWrite  (0, 255);

//    Serial.println("fgfdgsdg");
//    delay(1000);
//          ledcWrite(1, 255);
//    ledcWrite(0, 255);
//    delay(1000);
//       ledcWrite(1, 0);
//  ledcWrite  (0, 255);
//  Serial.println("ttttttttttt");
//    delay(1000);
//       ledcWrite(1, 255);
//    ledcWrite(0, 255);
//    delay(1000);
//       ledcWrite(1, 0);
//    ledcWrite(0, 0);
//    delay(1000);
// delay(2000);
//    pinMode(26, OUTPUT);
//    pinMode(25, OUTPUT);
//    digitalWrite(26, HIGH);
//    digitalWrite(25, HIGH);
//    delay(2000);
   // digitalWrite(26, LOW);
   // digitalWrite(25, LOW);
   
   // irrecv.enableIRIn();
   
}
void loop(){
   delay(10);
}


