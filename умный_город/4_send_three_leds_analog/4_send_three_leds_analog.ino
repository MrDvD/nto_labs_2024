#include <WiFi.h>

const char ssid[] = "Redmi Note 10S";
const char pass[] = "11111111";

const char ip[] = "192.168.6.242";
const uint16_t port = 7000;

void setup() {
   Serial.begin(115200);
   
   // PINS
   ledcSetup(0, 2048, 8);
   ledcSetup(1, 2048, 8);
   ledcSetup(2, 2048, 8);
   ledcAttachPin(16, 1);
   ledcAttachPin(25, 2);
   ledcAttachPin(26, 0);
   
   // WI-FI CONNECTION
   Serial.println("[WIFI_CONNECTION]");
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
   if (WiFi.waitForConnectResult()!= WL_CONNECTED) {
      Serial.println("[WIFI_FAIL]");
      delay(1000);
      ESP.restart();
   }
   Serial.printf("[WIFI_SUCCESS]");
}

// если замена верна, то эта функция бесполезна

// int to_decimal(std::string binary) {
//    int decimal = 0;
//    for (int i = 0; i < 8; i++) {
//       if (binary[i] == '1') {
//          decimal = 2 * decimal + 1;
//       } else {
//          decimal = 2 * decimal;
//       }
//    }
//    return decimal;
// }

void loop() {
   // SERVER CONNECTION
   WiFiClient client;
   Serial.printf("[SERVER_CONNECTION]: %s\n", ip);
   if (!client.connect(ip, port)){
      Serial.println("[SERVER_FAIL]");
      delay(2500);
      return;
   }
   Serial.println("[SERVER_SUCCESS]");
   
   // SERVER INTERACTION
   while(client.connect(ip, port)){
      client.printf("1");
      String response = client.readStringUntil('\n');
      Serial.println(response);
      // если сервер написан нормально, то нижний if не нужен
      // (были проблемы с асинхронной обработкой двух клиентов, см. main_server)
      if (response == "") {
         Serial.println("[EMPTY_RESPONSE]");
         break;
      }
      // PARSING BINARY TCP RESPONSE
      std::string response_str = response.c_str();
      for (int i = 0; i < 3; i++){
         std::string binary = response_str.substr(i * 8, i * 8 + 8);
         // заменил to_decimal(binary) на встроенную функцию --- не проверял
         int value = std::strtol(binary.c_str(), 0, 2);
         Serial.println(value);
         ledcWrite(i, value);
      }
      delay(750);
   }
   client.stop();
   Serial.println("[SERVER_DISCONNECT]");
   delay(2500);
}
