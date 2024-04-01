#include <BluetoothSerial.h>

BluetoothSerial espBT;

float lin = 0;

uint8_t address[6]  = {0x74, 0xC6, 0x3B, 0x38, 0xA4, 0xA0};

xTaskHandle task_g;

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   // =======================Bluetooth=====================
   Serial.println("[BT_CONNECTION]");
   espBT.begin("ESP32test", true);
   Serial.println("[STARTED_BLUETOOTH]");
   Serial.println("[BLUE_SUCCESS]");
   
   xTaskCreatePinnedToCore(getK, "get", 10000, NULL, 1, &task_g, 0);
}

void getK(void*params){
   while (true){
      lin += 0.01;
      delay(50);
   }
}

std::string packet = "";

void loop() {
   bool connected = espBT.connect(address, 1);
   while (!connected) {
      Serial.println("[BLUE_FAIL]");
      connected = espBT.connect(address, 1);
   }
   while (espBT.available()) {
      packet += espBT.read();
   }
   if (packet != "") {
      Serial.println(packet.c_str());
      packet = "";
   }
   espBT.print(lin);
   espBT.disconnect();
   delay(50);
}