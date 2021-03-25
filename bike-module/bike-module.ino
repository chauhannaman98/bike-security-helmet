// bike module set as AP to receive packets

#include <ESP8266WiFi.h>  
#include <WiFiUdp.h>
#include "ESPAsyncWebServer.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ssid "Bike-Module-AP"
#define password "qwerty123"
#define PORT 4210   // local port to listen on
#define SDA D2
#define SCL D1
#define buzzer D3
#define frequency 4000


AsyncWebServer server(80);
WiFiUDP Udp;
LiquidCrystal_I2C lcd(0x27, 16, 2);

char incomingPacket[255];  // buffer for incoming packets
char replyPacket[] = "Got message successfully!";  // a reply string to send back
//unsigned long Interval = 20000;
bool switchStatus = false;


void setup()  {
  Serial.begin(115200);
  Serial.println();

  Wire.begin(SDA, SCL);
  lcd.begin(16, 2);   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.home();

  Serial.print("Setting soft-AP ... ");
  WiFi.mode(WIFI_AP);
  Serial.println(WiFi.softAP(ssid, password) ? "Ready":"Failed!");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  Udp.begin(PORT);
  server.begin(); 
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), PORT);

  pinMode(buzzer, OUTPUT);
  // LED pin to show client status
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  lcd.clear();
  lcd.print("System ready!");
  delay(2000);
  
  lcd.clear();
}


void loop() {
  bool _client = client_status();
  lcd.setCursor(0, 0);
  lcd.print("Connection: ");
  lcd.setCursor(12, 0);
  if(_client)  {
//    Serial.printf("client: true\t");
    lcd.print("Okay");
    lcd.setCursor(0, 1);
    lcd.print("Ignition: ");
    receive_packets();
    if(switchStatus) {
//      Serial.printf("packet: true\n");
      noTone(buzzer);   // turn buzzer off
      lcd.setCursor(10, 1);
      lcd.print("ON ");
      digitalWrite(LED_BUILTIN, LOW); // turn led ON
    }
    else  {
//      Serial.printf("packet: false\n");
      tone(buzzer, frequency);  // turn buzzer on
      lcd.setCursor(10, 1);
      lcd.print("OFF");
      digitalWrite(LED_BUILTIN, HIGH);  //turn led OFF
    }
  }
  else  {
    tone(buzzer, frequency);    // turn buzzer on
    lcd.print("Lost");
    lcd.setCursor(0, 1);
    lcd.print("Ignition: OFF");
    switchStatus = false;
  }
}

bool client_status() {
  unsigned char number_client;  
  number_client= wifi_softap_get_station_num();
//  Serial.printf("Total Connected Clients are = %d\n", number_client);

  if(number_client == 0)  {
    return false;
  }
  else if(number_client > 0) {
    return true;
  }
}


void receive_packets()  {
  int packetSize = Udp.parsePacket();
//  Serial.printf("packetSize = %d\n", packetSize);
  if(packetSize) {
    // receive incoming UDP packets
//    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)  {
      incomingPacket[len] = 0;
    }
//    Serial.printf("UDP packet contents:%s\n", incomingPacket);
  
    if(strncmp(incomingPacket, "all_good", 4)==0)  {
      switchStatus = true;
    }
    else if(strncmp(incomingPacket, "drunk", 5)==0) {
      switchStatus = false;
    }
    else if(strncmp(incomingPacket, "no_helmet", 9)==0) {
      switchStatus = false;
    }
    else if(strncmp(incomingPacket, "not_okay", 8)==0) {
      switchStatus = false;
    }

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
  }
}
