// helmet module set as STA to send packets

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define ssid "Bike-Module-AP"
#define password "qwerty123"
#define IP "192.168.4.1"
#define PORT 4210   // local port to listen on

WiFiUDP Udp;

char senderPacket[100];


void setup() {
  Serial.begin(115200);
  Serial.println();

  // Connect to AP
  Serial.printf("Connecting to %s ", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  Udp.begin(PORT);
}


void loop() {
  while (Serial.available() > 0 ) {
    String inputString = Serial.readString();
    inputString.toCharArray(senderPacket, 100);
    if(Udp.beginPacket(IP, PORT) &&
      Udp.write(senderPacket) &&
      Udp.endPacket())  {
      Serial.printf("Packet sent: %s\n", senderPacket);
    }
  }
}
