// bike module set as AP to receive packets

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define ssid "Bike-Module-AP"
#define password "qwerty123"
#define PORT 4210   // local port to listen on

WiFiUDP Udp;

char incomingPacket[255];  // buffer for incoming packets
char replyPacket[] = "Got message successfully!";  // a reply string to send back


void setup()  {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready":"Failed!");
  
  Udp.begin(PORT);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), PORT);

  // LED pin to show client status
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}


void loop() {
  bool _client = client_status();
  if(_client)  {
    receive_packets();
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
//  digitalWrite(LED_BUILTIN, LOW); // client connected
  int packetSize = Udp.parsePacket();
  if(packetSize) {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)  {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents:%s\n", incomingPacket);
  
    if(strcmp(incomingPacket, "true")==10)  {
      digitalWrite(LED_BUILTIN, LOW); // turn led ON
    }
    else if(strcmp(incomingPacket, "false")==10) {
      digitalWrite(LED_BUILTIN, HIGH);  //turn led OFF
    }

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
  }
  else  {
//    digitalWrite(LED_BUILTIN, HIGH);  // client disconnected
  }
}
