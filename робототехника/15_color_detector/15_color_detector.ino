#include <Wire.h>
#include <cmath>
#include <BluetoothSerial.h>
#include <Adafruit_TCS34725.h>

BluetoothSerial espBT;

uint8_t address[6]  = {0x74, 0xC6, 0x3B, 0x38, 0xA4, 0xA0};

float kp = 105, ki=0, kd = 0, lin; 
bool mov = 0;

uint8_t i2c_addr = 0x2A;
int sensitivity = 208, delta;
float previousValue = 0.;
const uint8_t sensorPinMap[] = {
    4, 5, 6, 8, 7, 3, 2, 1
};

xTaskHandle  task_c;
// xTaskHandle task_g;

Adafruit_TCS34725 colorSensor;

struct RGB {
public:
    byte red;
    byte green;
    byte blue;
};

void writeData16(uint16_t command, uint16_t data){
   Wire.beginTransmission(i2c_addr);
   Wire.write(command);
   Wire.write((data >> 8) & 0xff);
   Wire.write(data & 0xff);
   Wire.endTransmission();
}
void writePin(uint8_t command, uint8_t pin, bool sendStop){
   Wire.beginTransmission(i2c_addr);
   Wire.write(command);
   Wire.write(pin);
   Wire.endTransmission(sendStop);
}

void pinMP(uint16_t val, uint8_t mode){
   switch (mode) {
   case INPUT:
      writeData16(0x04, val);
      break;
   case OUTPUT:
      writeData16(0x07, val);
      break;
   }
}
float patternToLine(uint8_t line) {
   switch (line) {
   case 0b00011000: return 0;
   case 0b00010000: return 0.25;
   case 0b00111000: return 0.25;
   case 0b00001000: return -0.25;
   case 0b00011100: return -0.25;
   case 0b00110000: return 0.375;
   case 0b00001100: return -0.375;
   case 0b00100000: return 0.5;
   case 0b01110000: return 0.5;
   case 0b00000100: return -0.5;
   case 0b00001110: return -0.5;
   case 0b01100000: return 0.625;
   case 0b11100000: return 0.625;
   case 0b00000110: return -0.625;
   case 0b00000111: return -0.625;
   case 0b01000000: return 0.75;
   case 0b11110000: return 0.75;
   case 0b00000010: return -0.75;
   case 0b00001111: return -0.75;
   case 0b11000000: return 0.875;
   case 0b00000011: return -0.875;
   case 0b10000000: return 1.0;
   case 0b00000001: return -1.0;
   default: return NAN;
   }
}

uint8_t mapAnalogToPattern(int16_t* analogValues){
   uint8_t pattern = 0;
   int16_t min = 20;
   int16_t max = 0;
   for (int i = 0; i < 8; i++) {
        if (analogValues[i] < min)
            min = analogValues[i];
        if (analogValues[i] > max)
            max = analogValues[i];
   }
   // std::string res = "";
   // for (int i = 0; i < 8; i++) {
   //    res += std::to_string(analogValues[i]);
   //    res += " ";
   // }
   // res += "threshold ";
   int threshold = min + (max - min) / 2;
   // res += std::to_string(threshold);
   for (int i = 7; i >= 0; i--) {
      pattern += pow(2, i) * ((analogValues[7 - i] < threshold) ? 0 : 1);
   }
   // res += " pattern ";
   // res += std::to_string(pattern);
   // Serial.printf("analogValues %s\n", res.c_str());
   return pattern;
}

void dWrite(uint8_t pin, bool val){
   uint16_t sendData = 1<<pin;
   if (val){
      writeData16(0x09, sendData);
   }else{
      writeData16(0x0A, sendData);
   }
}

int16_t read16Bit(){
   int16_t result = -1;
   const uint8_t byteCount = 2;
   Wire.requestFrom(i2c_addr, byteCount);
   if (Wire.available() != byteCount)
      return result;
   result = Wire.read();
   // Serial.println(result);
   // int16_t res2 = Wire.read();
   // Serial.println(res2);
   // result = (result<<8) | res2;
   return result;
}

int16_t anRead(uint8_t sensor){

   writePin(0x0C, sensor, true);
   int16_t result = read16Bit();
   // Serial.println((int) result);
   // if (result>=0)
      // result = result >> (12 - 10);
   // Serial.println(result);
   return result;
}

float trackLine(){
   int16_t analogValues[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
   for (uint8_t i = 0; i < 8; i++) {
      analogValues[i] = anRead(sensorPinMap[i]);
   }
   // std::string res = "";
   // for (int i = 0; i < 8; i++) {
   //    res += std::to_string(analogValues[i]);
   //    res += " ";
   // }
   // Serial.println(res.c_str());
   uint8_t pattern = mapAnalogToPattern(analogValues);
   // Serial.println(pattern);
   float result = patternToLine(pattern);
   result = isnan(result) ? previousValue : result;
   previousValue = result;
   return result;
}

int computePID(float input, float setpoint, float kp, float ki, float kd, float dt) {
  float err = setpoint - input;
  static float integral = 0, prevErr = 0;
  integral += err * dt;
  float D = (err - prevErr) / dt;
  prevErr = err;
  return (err * kp + integral * ki + D * kd);
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

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   // =======================Line Sensor begin=====================
   Wire.begin();
   writeData16(0x0D, 8000);
   pinMP(1<<9, OUTPUT);
   dWrite(9, HIGH);
   // =======================Line Sensor set sensitivity=====================
   ledcSetup(2, 5000, 8);
   ledcAttachPin(26, 0);
   ledcWrite(2, sensitivity);
   // =======================Bluetooth=====================
   Serial.println("[BT_CONNECTION]");
   espBT.begin("ESP32test", true);
   Serial.println("[STARTED_BLUETOOTH]");
   // bool connected = espBT.connect(address, 3);
   // while (!connected) {
   //    Serial.println("[BLUE_FAIL]");
   //    connected = espBT.connect(address, 3);
   // }
   Serial.println("[BLUE_SUCCESS]");
   

   colorSensor.begin();


   init_drive(27, 16, 14, 17);
   
   // xTaskCreatePinnedToCore(getK, "get", 10000, NULL, 1, &task_g, 0);
   xTaskCreatePinnedToCore(check, "check", 10000, NULL, 1, &task_c, 1);
}

RGB colorRead() {
    float red, green, blue;
    colorSensor.getRGB(&red, &green, &blue);
    
    RGB color;
    color.red = red;
    color.green = green;
    color.blue = blue;
    return color;
}

void check(void*params){
   ledcSetup(3, 5000, 8);
   ledcAttachPin(13,1);
   while (true){
      ledcWrite(3, analogRead(34)/4);
      RGB color = colorRead();
      Serial.print(color.red);
      Serial.print('\t');
      Serial.print(color.green);
      Serial.print('\t');
      Serial.println(color.blue);
      delay(100);
   }
}

void loop() {
   delay(10);
}