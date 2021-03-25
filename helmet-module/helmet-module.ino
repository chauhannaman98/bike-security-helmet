// helmet module set as STA to send packets

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define ssid "Bike-Module-AP"
#define password "qwerty123"
#define IP "192.168.4.1"
#define PORT 4210   // local port to listen on
#define MQ3 A0
#define ALCOHOL_THRESHOLD 500
#define IR_SENSOR D5

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
  pinMode(MQ3, INPUT);
  pinMode(IR_SENSOR, INPUT);
}


void loop() {
  String msgString;
  int alcohol_level = analogRead(MQ3);
  bool drunk = checkDrunk(alcohol_level);
  bool helmet = helmetWorn();
//  Serial.printf("IR: %d\t", helmet);
//  Serial.printf("Alcohol level: %d\tdrunk:%d\n", alcohol_level, drunk);

  if(helmet && !drunk) {
    msgString = "all_good";
  }
  else if(helmet && drunk) {
    msgString = "drunk";
  }
  else if(!helmet && !drunk)  {
    msgString = "no_helmet";
  }
  else  {
    msgString = "not_okay";
  }
    
  msgString.toCharArray(senderPacket, 100);
  if(Udp.beginPacket(IP, PORT) &&
    Udp.write(senderPacket) &&
    Udp.endPacket())  {
//    Serial.printf("Packet sent: %s\n", senderPacket);
  }
  delay(50);
}

bool checkDrunk(int alcoholLevel) {
  if(alcoholLevel > ALCOHOL_THRESHOLD)
    return true;
  else
    return false;
}

bool helmetWorn()  {
  if(digitalRead(IR_SENSOR) == 0) {
    return false;
  }
  else  {
    return true;
  }
}
