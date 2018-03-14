#include <SPI.h>
#include "RF24.h"
RF24 myRadio(4, 5);
byte addresses[][6] = {"0XXX0", "1PIR1", "2SHE2", "3SWI3", "4SWI4", "5SWI5"};
int thisSwitchToggleOnOffValue = 2000;
int thisSwitchExplicitON = 2111;
int thisSwitchExplicitOFF = 2222;
int dataReceived = 0;
int relay_pin = 3;
int button_pin = 8;
int button_LED = 9;
boolean light_state = false;

void setup()
{
  //Serial.begin(9600);
  pinMode(relay_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(button_LED, OUTPUT);
  digitalWrite(relay_pin, LOW);
  delay(1000);
  //Serial.print(myRadio.begin());
  myRadio.setChannel(111);
  myRadio.setAutoAck(1);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.openReadingPipe(1, addresses[4]);
  myRadio.startListening();
}

void loop()
{
  debug();
  radio_read();
  interpret_radio_message();
  button_switch();
  master_switch();
  digitalWrite(button_LED, !light_state);
}

void master_switch()
{
  if (dataReceived == thisSwitchExplicitON)
  {
    light_state = true;
    dataReceived = 0;
    digitalWrite(relay_pin, light_state);
  }
  else if (dataReceived == thisSwitchExplicitOFF)
  {
    light_state = false;
    dataReceived = 0;
    digitalWrite(relay_pin, light_state);
  }
}

void radio_read()
{
  if (myRadio.available())
  {
    myRadio.read(&dataReceived, sizeof(dataReceived));
    Serial.print("Data received = ");
    Serial.println(dataReceived);
  }
}

void interpret_radio_message()
{
  if (dataReceived == thisSwitchToggleOnOffValue)
  {
    light_state = !light_state;
    digitalWrite(button_LED, !light_state);
    digitalWrite(relay_pin, light_state);
    dataReceived = 0;
    delay(1000);
  }
}

void button_switch()
{
  if (digitalRead(button_pin) == LOW)
  {
    light_state = !light_state;
    digitalWrite(button_LED, !light_state);
    digitalWrite(relay_pin, light_state);
    delay(1000);
  }
}

void debug()
{
  Serial.print("is radio available:   ");
  Serial.println(myRadio.available());
}
