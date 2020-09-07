
/*
 * hacked up by justin@domain17.net
 * turn a pin into a switch and cycle hue lights through
 * several on/off/color configurations
 */

/*

 This example connects to an unencrypted Wifi network.
 Then it prints the  MAC address of the Wifi shield,
 the IP address obtained, and other network details.

 Circuit:
 * WiFi shield attached

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */

#define USE_ESP8266 1

#include "Arduino.h"
#include <SPI.h> 
 
#if USE_ESP8266
#include "ESP8266WiFi.h"
#else
#include <WiFiNINA.h>
#endif
 
//#include <Ethernet.h>

#define HUE_USERNAME "KMCUJg9FwBOiN2uJbfLirCnPQjbcBZKZu4RQvPCo"

char ssid[] = "dd-wrt";     //  your network SSID (name)
char pass[] = "brady123";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status


#if USE_ESP8266
int PIN_SWITCH = 13; // pin used for the switch (see https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)
int PIN_SIGNAL = LED_BUILTIN; // pin used for the LED
#else
int PIN_SWITCH = 3; // pin used for the switch
int PIN_SIGNAL = 1; // pin used for the LED
#endif

byte hueBridgeAddr[] = {192, 168, 1, 183};
int led_last = HIGH;
int booted = 0;

void wifiSetup(void);

void setup() {

  // set pin to read
  pinMode(PIN_SWITCH, INPUT);
  pinMode(PIN_SIGNAL, OUTPUT);
  
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  
  #if !USE_ESP8266
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  #else
  //Serial.println("skipping WiFi status check");
  #endif

  //String fv = WiFi.firmwareVersion();
  //if (fv != "1.1.0") {
  //  Serial.println("Please upgrade the firmware");
  //}
}

void wifiSetup(void)
{
  int retries = 4;
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED && retries > 0) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(/*10000*/4000);

    retries -= 1;
  }
}

void loop() {

  if (!booted) {
    Serial.println("dwah");
    digitalWrite(PIN_SIGNAL, LOW);
    delay(3000);
    digitalWrite(PIN_SIGNAL, HIGH);
    delay(3000);
    digitalWrite(PIN_SIGNAL, LOW);
    booted = 1;
  }
  
  // check the network connection once every 10 seconds:
  delay(1000);

  printCurrentNet();
  printWifiData();

  if (digitalRead(PIN_SWITCH) == 1) {
    bool onOff = false;
    
    onOff = queryHues();
    commandHues(onOff);

    Serial.println("button pressed");

    delay(5000);
  }
  else {

    status = WiFi.status();
    
    if(status == WL_CONNECTED) {
      Serial.println("flashing LED ");
      digitalWrite(PIN_SIGNAL, led_last);
    
      led_last = led_last == LOW ? HIGH : LOW;
    }
    else {
      wifiSetup();
    }
    delay(1000);
  }
}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {

  if(status != WL_CONNECTED) return;
  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  
  #if USE_ESP8266
  //memcpy(bssid, WiFi.BSSID(0), sizeof(bssid));
  #else
  WiFi.BSSID(bssid);
  #endif
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  #if USE_ESP8266
  byte encryption = WiFi.encryptionType(0);
  #else
  byte encryption = WiFi.encryptionType();
  #endif
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

int queryHues() {
  int result = 1;
  WiFiClient client;
  String buf;
  
  if(!client.connect(hueBridgeAddr, 80)) {
    Serial.print("connection to hue bridge failed");
    return 0;
  }

  String hueCmd = "GET /api/"HUE_USERNAME"/lights/1/ HTTP/1.1";

  client.println(hueCmd);
  client.println("Content-Length: 0");
  client.println("Content-Type: text/plain;charset=UTF-8");
  client.println(""); 
  client.println(""); 

  delay(1000);
  
  while (client.available()) {
    char c = client.read();
    buf += c;
  }

  Serial.print(buf);

  if (buf.indexOf("\"on\":true") > 0) {
    result = 0;
  }

  client.stop();

  Serial.print("query done\n");

  return result;
}

int commandHues(bool on) {
  int err = 0;
  WiFiClient client;

  for (int lightNum = 1; lightNum <= 3; lightNum ++) {

    if(!client.connect(hueBridgeAddr, 80)) {
      err = -1;
      Serial.print("connection to hue bridge failed");
      break;
    }

    String hueCmdBodyOn = "{\"on\": true,\"hue\":10000,\"sat\":255,\"bri\":255}";
    String hueCmdBodyOff = "{\"on\": false}";
    String hueCmd = "PUT /api/"HUE_USERNAME"/lights/"+String(lightNum)+"/state HTTP/1.1";
    String strBuf;
    String body = on ? hueCmdBodyOn : hueCmdBodyOff;
    int bLen = body.length();
  
    client.println(hueCmd);
    strBuf = "Content-Length: " + String(bLen);
    client.println(strBuf);
    client.println("Content-Type: text/plain;charset=UTF-8");
    client.println("");
    client.println(body);

    Serial.print("writing " + hueCmd + strBuf + body + "\n successful\n");
    
    delay(2000);

    Serial.print("response:\r\n");
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    Serial.print("\r\n");

    delay(1000);
    
    client.stop();
  }

  return err;
}
