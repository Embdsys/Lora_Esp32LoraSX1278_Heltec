#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"
 
SSD1306  display(0x3c, 4, 15);
 
//OLED pins to ESPa32 GPIOs via this connection:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16
// WIFI_LoRa_32 ports
// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)
 
#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 does not work for ESP32LoraV2Heltec (Asia mode)

bool flag = false;

void setup() {
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
   
  Serial.begin(115200);
  while (!Serial); //if just the the basic function, must connect to a computer
  delay(1000);
  Serial.println("LoRa Receiver"); 
  display.drawString(5,5,"LoRa Receiver"); 
  display.display();
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
   
  if (!LoRa.begin(BAND)) {
    display.drawString(5,25,"Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  display.drawString(5,25,"LoRa Initializing OK!");
  display.display();
}

void toggleIO(String whichIO) {
  if (whichIO && flag == false) {
    flag = true;
    pinMode(12,OUTPUT); // toggleIO()
    pinMode(13,OUTPUT); // toggleIO()
}
  if (whichIO == "Hello..10"){
    digitalWrite(12, !digitalRead(12));
  }else if (whichIO == "Hello..5"){
    digitalWrite(13, !digitalRead(13));
  } 
}

void loop() {
  // try to aparse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packets
//    Serial.print("Received packet. ");
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(3, 0, "Received packet ");
    display.display();
    // read packet
    while (LoRa.available()) {
    String data = LoRa.readString();
    Serial.print(data);
    display.drawString(20,22, data); // data contains the incoming message //
    display.display();
    toggleIO(data); // function that selects which IO to trigger
    }
    // print RSSI of packet
//    Serial.print(" with RSSI ");
//    Serial.println(LoRa.packetRssi());
    display.drawString(20, 45, "RSSI:  ");
    display.drawString(70, 45, (String)LoRa.packetRssi());
    display.display();
  }
}

//https://escapequotes.net/esp32-lora-send-receive-packet-test/
