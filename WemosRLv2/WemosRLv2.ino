#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "Switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
const byte value1_pin = D1;
const byte value2_pin = D2;
const byte value3_pin = D3;
const byte value4_pin = D4;
const byte value5_pin = D5;
const byte actionPin = D6;
boolean connectWifi();
byte codes[32][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 1, 1},
    {0, 0, 1, 0, 0}, //numbers will be converted from binary to
    {0, 0, 1, 0, 1}, //decimal by connected arduino
    {0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1},
    {0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 1, 0, 1, 1},
    {0, 1, 1, 0, 0},
    {0, 1, 1, 0, 1},
    {0, 1, 1, 1, 0},
    {0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 1, 1},
    {1, 0, 1, 0, 0},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 1, 0},
    {1, 0, 1, 1, 1},
    {1, 1, 0, 0, 0},
    {1, 1, 0, 0, 1},
    {1, 1, 0, 1, 0},
    {1, 1, 0, 1, 1},
    {1, 1, 1, 0, 0},
    {1, 1, 1, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1}};
void workLightsOn();
void workLightsOff();
void livingLightsOn();
void livingLightsOff();
void bedroomLightsOn();
void bedroomLightsOff();
void masterLightsOn();
void masterLightsOff();
void diningLightsOn();
void diningLightsOff();

const char *ssid = "---------";
const char *password = "-------";
boolean wifiConnected = false;
UpnpBroadcastResponder upnpBroadcastResponder;
Switch *bedroom = NULL;
Switch *living = NULL;
Switch *dining = NULL;
Switch *master = NULL;
Switch *work = NULL;

void setup()
{
  Serial.begin(9600);
  pinMode(value1_pin, OUTPUT);
  pinMode(value2_pin, OUTPUT);
  pinMode(value3_pin, OUTPUT);
  pinMode(value4_pin, OUTPUT);
  pinMode(value5_pin, OUTPUT);
  pinMode(actionPin, OUTPUT);
  wifiConnected = connectWifi();
  if (wifiConnected)
  {
    upnpBroadcastResponder.beginUdpMulticast();
    bedroom = new Switch("bedroom lights", 82, bedroomLightsOn, bedroomLightsOff); //Alexa invocation name, local port no, on callback, off callback
    living = new Switch("livingroom lights", 81, livingLightsOn, livingLightsOff);
    dining = new Switch("diningroom lights", 83, diningLightsOn, diningLightsOff);
    master = new Switch("house lights", 84, masterLightsOn, masterLightsOff);
    work = new Switch("work lights", 80, workLightsOn, workLightsOff);
    //Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*bedroom);
    upnpBroadcastResponder.addDevice(*living);
    upnpBroadcastResponder.addDevice(*dining);
    upnpBroadcastResponder.addDevice(*master);
    upnpBroadcastResponder.addDevice(*work);
  }
}

void loop()
{
  if (wifiConnected)
  {
    upnpBroadcastResponder.serverLoop();
    bedroom->serverLoop();
    living->serverLoop();
    dining->serverLoop();
    master->serverLoop();
    work->serverLoop();
  }
}

void diningLightsOn()
{
  setPins(codes[1][0], codes[1][1], codes[1][2], codes[1][3], codes[1][4]);
}

void diningLightsOff()
{
  setPins(codes[2][0], codes[2][1], codes[2][2], codes[2][3], codes[2][4]);
}

void workLightsOn()
{
  setPins(codes[3][0], codes[3][1], codes[3][2], codes[3][3], codes[3][4]);
}

void workLightsOff()
{
  setPins(codes[4][0], codes[4][1], codes[4][2], codes[4][3], codes[4][4]);
}

void livingLightsOn()
{
  setPins(codes[5][0], codes[5][1], codes[5][2], codes[5][3], codes[5][4]);
}

void livingLightsOff()
{
  setPins(codes[6][0], codes[6][1], codes[6][2], codes[6][3], codes[6][4]);
}

void bedroomLightsOn()
{
  setPins(codes[7][0], codes[7][1], codes[7][2], codes[7][3], codes[7][4]);
}

void bedroomLightsOff()
{
  setPins(codes[8][0], codes[8][1], codes[8][2], codes[8][3], codes[8][4]);
}

void masterLightsOn()
{
  setPins(codes[9][0], codes[9][1], codes[9][2], codes[9][3], codes[9][4]);
}

void masterLightsOff()
{
  setPins(codes[10][0], codes[10][1], codes[10][2], codes[10][3], codes[10][4]);
}

void setPins(byte setPin1, byte setPin2, byte setPin3, byte setPin4, byte setPin5)
{
  digitalWrite(D1, setPin1);
  digitalWrite(D2, setPin2);
  digitalWrite(D3, setPin3);
  digitalWrite(D4, setPin4);
  digitalWrite(D5, setPin5);
  delay(20);
  digitalWrite(actionPin, HIGH);
  delay(200);
  digitalWrite(actionPin, LOW);
}

boolean connectWifi()
{
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Serial.println("");
  //Serial.println("Connecting to WiFi");
  // Wait for connection
  //Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //Serial.print(".");
    if (i > 10)
    {
      state = false;
      break;
    }
    i++;
  }

  //if (state) {
  // Serial.println("");
  // Serial.print("Connected to ");
  // Serial.println(ssid);
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
  //}
  // else {
  // Serial.println("");
  //Serial.println("Connection failed.");
  //}
  return state;
}
