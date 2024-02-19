#include <WiFi.h>
#include <WiFiUdp.h>
#include <cmath>
#include <bitset>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char addr[] = "192.168.45.242";
int port = 7001;

WiFiUDP udp;

std::string pack;
int clicker = 0;
int user = 0;
int pressed = 0;
bool bWasUp = false, bWasUp2 = false;

TaskHandle_t task_cl, task_send;

// SemaphoreHandle_t mtx = xSemaphoreCreateMutex();

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
      bool isup = digitalRead(26);
      Serial.printf("\r");
      if (!bWasUp2 && isup){
         pack = float_to_binary(user, 1) + float_to_binary(clicker, 1);
         udp.beginPacket(addr, port);
         udp.println(pack.c_str());
         udp.endPacket();
         Serial.println(pack.c_str());
         user++;
         user %= 4;
         clicker = 0;
         pressed = 0;
      }
      bWasUp2 = isup;
   }
}

void clicks(void *params){
   while (true){
      bool isup = digitalRead(25);
      if (bWasUp && !isup){
         clicker += 1;
      }
      bWasUp = isup;
      
   }
}

void setup() {
   Serial.begin(115200);
   
   pinMode(26, INPUT_PULLDOWN);
   pinMode(25, INPUT);
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
   
   
   xTaskCreatePinnedToCore(send, "s", 10000, NULL, 1, &task_send, 0);
   xTaskCreatePinnedToCore(clicks, "c", 10000, NULL, 1, &task_cl, 1);
}

void loop() {
   vTaskDelay(100);
}
