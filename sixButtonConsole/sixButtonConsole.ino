
#include <SPI.h>    // Comes with Arduino IDE
#include "RF24.h"
RF24 myRadio(2, 3); //chip enable and chip select, configurable
byte addresses[][6] = {"0XXX0", "1PIR1", "2SHE2", "3SWI3", "4SWI4", "5SWI5"};
/*
  0XXX0 - reference adjust placeholder
  1PIR1 - PIR sensor unit 1
  2SHE2 - the shelf unit - controls indepent relays for dining room and bedroom lights (the chandeleirs)
  3SWI3 - smart switch which will control the light tower/lamp in the living room
  4SWI4 - additional smart switch
  5SWI5 - additional smart switch
*/
int light_number_message = 0;
const byte zone1_button = 4;
const byte zone2_button = 5;
const byte zone3_button = 7;
const byte zone4_button = 8;
const byte motion_button = 6;
const byte master_button = 10;
int retry_number = 1;
int green_led_pin = 9;
int red_led_pin = 17;
int number_message_array[9] = {0000, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000};
int radio_OFF_message_array[9] = {4321, 1111, 2222, 3222, 4222, 5222, 6222, 7222, 8222};
int radio_ON_message_array[9] = {1234, 1222, 2111, 3111, 4111, 5111, 6111, 7111, 8111};
boolean motionToggle = true;
boolean master_state = false;
boolean zone1_button_state = false;
boolean zone2_button_state = false;
boolean zone3_button_state = false;
boolean zone4_button_state = false;
boolean motion_button_state = false;

void setup()
{
  pinMode(zone1_button, INPUT_PULLUP);
  pinMode(zone2_button, INPUT_PULLUP);
  pinMode(zone3_button, INPUT_PULLUP);
  pinMode(zone4_button, INPUT_PULLUP);
  pinMode(motion_button, INPUT_PULLUP);
  pinMode(master_button, INPUT_PULLUP);
  pinMode(green_led_pin, OUTPUT);
  pinMode(red_led_pin, OUTPUT);
  analogWrite(green_led_pin, 20);
  Serial.begin(9600);
  delay(1000);
  myRadio.begin();
  myRadio.setAutoAck(1);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setChannel(111); // 2.400GHz to 2.524GHz, 0-123
  myRadio.setPALevel(RF24_PA_MAX);
  delay(1000);
}

void loop()
{
  // debug();
  multi_button_poll();
  master_button_function();
}

void multi_button_poll()
{

  single_button_poll(zone1_button, 1, 2);
  single_button_poll(zone2_button, 2, 3);
  single_button_poll(zone3_button, 3, 2);
  single_button_poll(zone4_button, 4, 4);
  single_button_poll(motion_button, 8, 1);
}

void master_button_function()
{
  if (digitalRead(master_button) == LOW)
  {
    LED_blink();
    master_state = !master_state;
    if (master_state == true)
    {
      myRadio.openWritingPipe(addresses[2]);
      myRadio.write(&radio_ON_message_array[0], sizeof(radio_ON_message_array[0]));
      myRadio.openWritingPipe(addresses[3]);
      myRadio.write(&radio_ON_message_array[2], sizeof(radio_ON_message_array[2]));
      myRadio.openWritingPipe(addresses[4]);
      myRadio.write(&radio_ON_message_array[4], sizeof(radio_ON_message_array[4]));
    }
    else if (master_state == false)
    {
      myRadio.openWritingPipe(addresses[2]);
      myRadio.write(&radio_OFF_message_array[0], sizeof(radio_OFF_message_array[0]));
      myRadio.openWritingPipe(addresses[3]);
      myRadio.write(&radio_OFF_message_array[2], sizeof(radio_OFF_message_array[2]));
      myRadio.openWritingPipe(addresses[4]);
      myRadio.write(&radio_OFF_message_array[4], sizeof(radio_OFF_message_array[4]));
    }
    delay(800);
  }
}

void single_button_poll(const byte button_name, int messages_array_value, int address_array_value)
{
  if (digitalRead(button_name) == LOW)
  {
    LED_blink();
    light_number_message = number_message_array[messages_array_value];
    myRadio.openWritingPipe(addresses[address_array_value]);
    myRadio.write(&light_number_message, sizeof(light_number_message));
    delay(800);
  }
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

void debug()
{
  Serial.print("zone1 pin read:  ");
  Serial.println(digitalRead(zone1_button));
  Serial.print("zone2 pin read:  ");
  Serial.println(digitalRead(zone2_button));
  Serial.print("zone3 pin read:  ");
  Serial.println(digitalRead(zone3_button));
  Serial.print("zone4 pin read:  ");
  Serial.println(digitalRead(zone4_button));
  Serial.print("motion pin read:  ");
  Serial.println(digitalRead(motion_button));
  Serial.print("master pin read:  ");
  Serial.println(digitalRead(master_button));
}
