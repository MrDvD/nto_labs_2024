#include <WiFi.h>
#include <WiFiUdp.h>
#include <cmath>
#include <bitset>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char addr[] = "192.168.76.242";
int port = 7001;

long duration;
float cm;

IPAddress dest;
WiFiUDP udp;

TaskHandle_t task_get, task_send;


struct Avg {
    int pin1, pin2, samples, ms;
    float sum;
    float get() {
        for (int i = 0; i < samples; i++) {
               digitalWrite(pin1, LOW);
               delayMicroseconds(5);
               digitalWrite(pin1, HIGH);
               
               delayMicroseconds(10);
               digitalWrite(pin1, LOW);
   
               duration = pulseIn(pin2, HIGH);
         
            sum += duration;
            delay(ms);
        }
        float average = sum / samples; sum = 0;
        return average;
    }
    Avg(int p, int p2, int s, int m) {
        pin1 = p; pin2 = p2; samples = s; ms = m;
    }
};

auto avg_10 = Avg(26, 25, 5, 50);

std::string float_to_binary(float num, int precision) {
    std::string binarized;
    float integer;
    float fraction = std::round(std::modf(num, &integer) * pow(10, precision));
    integer = abs(integer); fraction = abs(fraction);
    if (fraction > 0) { // CUTS FRACTION'S TRAILING ZEROS
        while ((int) fraction % 10 == 0) {
            fraction /= 10;
        }
    }
    binarized += num >= 0 ? '0' : '1'; // SIGN
    if (fraction == 0) {
        binarized += "0"; // EXP_SIGN
        int zeros = 0;
        if (integer != 0) {
            while ((int) integer % 10 == 0) {
                integer /= 10;
                zeros++;
            }
        }
        binarized += std::bitset<6>(zeros).to_string(); // EXPONENT
        binarized += std::bitset<24>(integer).to_string(); // MANTISSA
    } else {
        binarized += "1"; // EXP_SIGN
        if (precision > ceil(log10(fraction))) { // CUTS MANTISSA'S TRAILING ZEROS
            precision = ceil(log10(fraction));
        }
        binarized += std::bitset<6>(precision).to_string(); // EXPONENT
        binarized += std::bitset<24>(integer * pow(10, precision) + fraction).to_string(); // MANTISSA
    }
    return binarized;
}

void send(void *params){
   while (true){
         cm = (avg_10.get() / 2) / 29.1;
         Serial.println(cm);
         udp.beginPacket("192.168.76.242", 7001);
         udp.println(float_to_binary(cm, 1).c_str());
         udp.endPacket();
         delay(500);
   }
}


void get(void *params){
   uint8_t buffer[1];
   while (true){
      udp.parsePacket();
      if(udp.read(buffer, 1) > 0){
         Serial.println((char *)buffer);
         if (buffer[0] == '0'){
            digitalWrite(17, LOW);
         }
         else {
            digitalWrite(17, HIGH);
         }
      }
   }
}



void setup() {
   Serial.begin(115200);
   
   pinMode(26, OUTPUT);
   pinMode(25, INPUT);
   pinMode(17, OUTPUT);
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
   
   xTaskCreatePinnedToCore(get, "g", 10000, NULL, 1, &task_get, 1);
   xTaskCreatePinnedToCore(send, "s", 10000, NULL, 1, &task_send, 0);
}

void loop() {
   vTaskDelay(100);
   }
