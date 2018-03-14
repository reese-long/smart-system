#include <SPI.h>
#include "RF24.h"
RF24 myRadio(4, 5);
byte addresses[][6] = {"0XXX0", "1PIR1", "2SHE2", "3SWI3", "4SWI4", "5SWI5"};
int number_message_array[9] = {0000, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000};
int radio_OFF_message_array[9] = {4321, 1111, 2222, 3222, 4222, 5222, 6222, 7222, 8222};
int radio_ON_message_array[9] = {1234, 1222, 2111, 3111, 4111, 5111, 6111, 7111, 8111};
int thisSwitchToggleOnOffValue_LR = 3000;
int thisSwitchToggleOnOffValue_BR = 1000;
int thisSwitchExplicitON_LR = 3111;
int thisSwitchExplicitOFF_LR = 3222;
int thisSwitchExplicitON_BR = 1111;
int thisSwitchExplicitOFF_BR = 1222;
int thisSwitchBothRelaysON = 1234;
int thisSwitchBothRelaysOFF = 4321;
int dataReceived = 0;
int relay_pin_BR = 6;
int relay_pin_LR = 7;
int button_pin_BR = 8;
int button_pin_LR = 9;
int button_pin_MASTER_LIGHTS = 10;
int red_led_pin = 2;
int green_led_pin = 3;
int master_ON_message = 10000;
int master_OFF_message = 00001;
int numAvail = 0;
boolean light_state_LR = false;
boolean light_state_BR = false;
boolean master_state = true;

void setup()
{
  Serial.begin(9600);
  pinMode(relay_pin_BR, OUTPUT);
  pinMode(relay_pin_LR, OUTPUT);
  pinMode(button_pin_MASTER_LIGHTS, INPUT_PULLUP);
  pinMode(button_pin_BR, INPUT_PULLUP);
  pinMode(button_pin_LR, INPUT_PULLUP);
  pinMode(green_led_pin, OUTPUT);
  pinMode(red_led_pin, OUTPUT);
  delay(1000);
  myRadio.begin();
  myRadio.setAutoAck(1);
  myRadio.setChannel(111);
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.openReadingPipe(1, addresses[2]);
  myRadio.setDataRate(RF24_250KBPS);
  analogWrite(green_led_pin, 20);
  myRadio.startListening();
}

void loop()
{
  radio_read();
  radio_switch();
  button_switch();
  master_button_transmit();
  digitalWrite(relay_pin_BR, light_state_BR);
  digitalWrite(relay_pin_LR, light_state_LR);
}

void LED_blink()
{
  digitalWrite(green_led_pin, LOW);
  digitalWrite(red_led_pin, HIGH);
  delay(50);
  digitalWrite(red_led_pin, LOW);
  delay(50);
  digitalWrite(red_led_pin, HIGH);
  delay(50);
  digitalWrite(red_led_pin, LOW);
  delay(50);
  digitalWrite(red_led_pin, HIGH);
  delay(50);
  digitalWrite(red_led_pin, LOW);
  analogWrite(green_led_pin, 20);
}

void master_button_transmit()
{
  if (digitalRead(button_pin_MASTER_LIGHTS) == LOW)
  {
    LED_blink();
    myRadio.stopListening();
    master_state = !master_state;
    light_state_BR = master_state;
    light_state_LR = master_state;
    digitalWrite(relay_pin_BR, light_state_BR);
    digitalWrite(relay_pin_LR, light_state_LR);
    if (master_state == true)
    {
      myRadio.openWritingPipe(addresses[2]);
      for (int i = 0; i < 20; i++)
      {
        myRadio.write(&radio_ON_message_array[2], sizeof(radio_ON_message_array[2]));
      }
      myRadio.openWritingPipe(addresses[3]);
      for (int i = 0; i < 20; i++)
      {
        myRadio.write(&radio_ON_message_array[4], sizeof(radio_ON_message_array[4]));
      }
    }
    if (master_state == false)
    {
      myRadio.openWritingPipe(addresses[2]);
      for (int i = 0; i < 100; i++)
      {
        myRadio.write(&radio_OFF_message_array[2], sizeof(radio_OFF_message_array[2]));
      }
      myRadio.openWritingPipe(addresses[3]);
      for (int i = 0; i < 100; i++)
      {
        myRadio.write(&radio_OFF_message_array[4], sizeof(radio_OFF_message_array[4]));
      }
    }
    myRadio.startListening();
    delay(500);
  }
}

void radio_read()
{
  for (int i = 0; i < 10; i++)
  {
    if (myRadio.available())
    {
      myRadio.read(&dataReceived, sizeof(dataReceived));
      Serial.print("Data received = ");
      Serial.println(dataReceived);
      break;
    }
  }
}

void radio_switch()
{
  if (dataReceived == thisSwitchToggleOnOffValue_LR)
  {
    light_state_LR = !light_state_LR;
    dataReceived = 0;
    LED_blink();
    delay(200);
  }
  if (dataReceived == thisSwitchToggleOnOffValue_BR)
  {
    light_state_BR = !light_state_BR;
    dataReceived = 0;
    LED_blink();
    delay(200);
  }
  if (dataReceived == thisSwitchExplicitOFF_LR)
  {
    light_state_LR = false;
    dataReceived = 0;
    LED_blink();
    delay(500);
  }
  if (dataReceived == thisSwitchExplicitON_LR)
  {
    light_state_LR = true;
    dataReceived = 0;
    LED_blink();
    delay(500);
  }
  if (dataReceived == thisSwitchExplicitOFF_BR)
  {
    light_state_BR = false;
    dataReceived = 0;
    delay(500);
  }
  if (dataReceived == thisSwitchExplicitON_BR)
  {
    light_state_BR = true;
    dataReceived = 0;
    LED_blink();
    delay(500);
  }
  if (dataReceived == thisSwitchBothRelaysON)
  {
    light_state_BR = true;
    light_state_LR = true;
    digitalWrite(relay_pin_BR, light_state_BR);
    digitalWrite(relay_pin_LR, light_state_LR);
    dataReceived = 0;
    LED_blink();
    delay(500);
  }
  if (dataReceived == thisSwitchBothRelaysOFF)
  {
    light_state_BR = false;
    light_state_LR = false;
    digitalWrite(relay_pin_BR, light_state_BR);
    digitalWrite(relay_pin_LR, light_state_LR);
    dataReceived = 0;
    LED_blink();
    delay(500);
  }
}

void button_switch()
{
  if (digitalRead(button_pin_BR) == LOW)
  {
    LED_blink();
    light_state_BR = !light_state_BR;
    digitalWrite(relay_pin_BR, light_state_BR);
    delay(1000);
  }
  if (digitalRead(button_pin_LR) == LOW)
  {
    LED_blink();
    light_state_LR = !light_state_LR;
    digitalWrite(relay_pin_LR, light_state_LR);
    delay(1000);
  }
}

void debug()
{
  Serial.println("");
  Serial.print(" BR BUTTON PIN READ:  ");
  Serial.println(digitalRead(button_pin_BR));
  Serial.println("");
  Serial.print(" LR BUTTON PIN READ:  ");
  Serial.println(digitalRead(button_pin_LR));
  Serial.println("");
  Serial.print(" MASTER BUTTON PIN READ:  ");
  Serial.println(digitalRead(button_pin_MASTER_LIGHTS));
}
