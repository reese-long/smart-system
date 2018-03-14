
#include <SPI.h>  // Comes with Arduino IDE
#include "RF24.h" // Download and Install (See above)

RF24 myRadio(8, 9); //chip enable and chip select, configurable

byte addresses[][6] = {"0XXX0", "2SHE2", "2SHE2", "4SWI4", "4SWI4", "3SWI3", "3SWI3", "2SHE2", "2SHE2"};
int messages[] = {0000, 3111, 3222, 4111, 4222, 2111, 2222, 1111, 1222 /* 5111, 5222, 6111, 6222, 7111, 7222 */};
/*
  0XXX0 - reference adjust placeholder
  1PIR1 - PIR sensor unit 1
  2SHE2 - the shelf unit - controls indepent relays for dining room and bedroom lights (the chendeleirs)
  3SWI3 - smart switch which will control the light tower/lamp in the living room
  4SWI4 - additional smart switch
  5SWI5 - additional smart switch
*/
int light_number_message = 0;
const byte value1_pin = 2;
const byte value2_pin = 3;
const byte value3_pin = 4;
const byte value4_pin = 5;
const byte value5_pin = 6;
const byte actionPin = 7;
const byte buzzer_pin = 14;
int actionMessageReceived;
boolean newMessage = false;

void setup()
{
  pinMode(value1_pin, INPUT);
  pinMode(value2_pin, INPUT);
  pinMode(value3_pin, INPUT);
  pinMode(value4_pin, INPUT);
  pinMode(value5_pin, INPUT);
  pinMode(actionPin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
  myRadio.begin();
  myRadio.setAutoAck(1);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setChannel(111); // 2.400GHz to 2.524GHz, 0-123
  myRadio.setPALevel(RF24_PA_MAX);
  //Serial.begin(9600);
  delay(1000);
}

void loop()
{
  echoReceive();
  interpretEchoMessage();
}

void echoReceive()
{
  if (digitalRead(actionPin) == HIGH)
  {
    actionMessageReceived = 0;
    actionMessageReceived = ((digitalRead(2) << 4) + (digitalRead(3) << 3) + (digitalRead(4) << 2) + (digitalRead(5) << 1) + digitalRead(6));
    //Serial.println(actionMessageReceived);
    newMessage = true;
  }
}

void interpretEchoMessage()
{
  if (newMessage == true)
  {
    if (actionMessageReceived == 10)
    {
      masterLightsOFF();
    }
    else if (actionMessageReceived == 9)
    {
      masterLightsON();
    }
    else
    {
      myRadio.openWritingPipe(addresses[actionMessageReceived]);
      myRadio.write(&messages[actionMessageReceived], sizeof(messages[actionMessageReceived]));
      newMessage = false;
      //actionMessage = 0;
    }
  }
}

void masterLightsON()
{
  // bedroom:
  myRadio.openWritingPipe(addresses[7]);
  myRadio.write(&messages[7], sizeof(messages[7]));
  // living room:
  myRadio.openWritingPipe(addresses[5]);
  myRadio.write(&messages[5], sizeof(messages[5]));
  //  dining room
  myRadio.openWritingPipe(addresses[1]);
  myRadio.write(&messages[1], sizeof(messages[1]));
  // work lights
  myRadio.openWritingPipe(addresses[3]);
  myRadio.write(&messages[3], sizeof(messages[3]));
  newMessage = false;
}

void masterLightsOFF()
{
  myRadio.openWritingPipe(addresses[7]);
  myRadio.write(&messages[8], sizeof(messages[8]));
  // living room:
  myRadio.openWritingPipe(addresses[5]);
  myRadio.write(&messages[6], sizeof(messages[6]));
  //  dining room
  myRadio.openWritingPipe(addresses[1]);
  myRadio.write(&messages[2], sizeof(messages[2]));
  //work lights
  myRadio.openWritingPipe(addresses[3]);
  myRadio.write(&messages[4], sizeof(messages[4]));
  newMessage = false;
}
