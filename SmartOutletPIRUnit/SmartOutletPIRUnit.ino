//////////////////////////////////////////////----------------START------------------//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <SPI.h>
#include "RF24.h"
#include "FastLED.h"
RF24 myRadio(15, 16);
byte addresses[][6] = {"0XXX0", "1PIR1", "2SWI2", "3PLE3", "4SHE4"}; // Create address for 1 pipe.
int dataReceived = 0;
int light_ON_number_message = 1111;
int light_OFF_number_message = 1222;
const byte pir_pin = 8;
const byte button_LED = 6;
int pir_on_off_button = 5;
int PIR_toggle_ON_radio_message = 5111;
int PIR_toggle_OFF_radio_message = 5222;
boolean radio_master_PIR_state = false;
boolean local_PIR_state = false;
boolean timer_state = false;
long int timer_maximum = 10000;
long int start_millis = 0;
long int motion_debounce_time = 8000;
long int motion_start_millis = 0;
boolean motion_timer = false;
boolean noMotion = true;
#define DATA_PIN 3
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
#define BRIGHTNESS 100
#define FRAMES_PER_SECOND 120

void setup()
{
  //Serial.begin(9600);
  pinMode(pir_pin, INPUT);
  pinMode(pir_on_off_button, INPUT_PULLUP);
  pinMode(button_LED, OUTPUT);
  // Serial.println(myRadio.begin());
  // Serial.println("");
  //   Serial.println("");
  myRadio.setChannel(111);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.openWritingPipe(addresses[4]);
  myRadio.openReadingPipe(1, addresses[1]);
  myRadio.startListening();
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  delay(1000);
}

void loop()

{
  //debug();
  local_switch_PIR_on_off();
  LED_control();
  if (digitalRead(pir_on_off_button) == LOW)
  {
    if (noMotion == true)
    {
      noMotion = false;
      radio_master_PIR_state = true;
      local_switch_PIR_on_off();
      LED_control();
    }
    sense_and_report();
    local_switch_PIR_on_off();
    radio_master_PIR_on_off();
    LED_control();
  }
  else
  {
    noMotion = true;
    LED_control();
  }
}

void radio_master_PIR_on_off()
{
  if (myRadio.available())
  {
    myRadio.read(&dataReceived, sizeof(dataReceived));
    //Serial.print("Data received = ");
    // Serial.println(dataReceived);
  }
  else
  {
    //Serial.println("radio waiting for message...");
  }
  if (dataReceived == PIR_toggle_ON_radio_message)
  {
    radio_master_PIR_state = true;
    dataReceived = 0;
    delay(500);
  }
  else if (dataReceived == PIR_toggle_OFF_radio_message)
  {
    radio_master_PIR_state = false;
    dataReceived = 0;
    delay(500);
  }
}

void LED_control()
{
  if (local_PIR_state == false || radio_master_PIR_state == false)
  {
    leds[0] = CRGB(0, 255, 00); //RED
    FastLED.show();
  }
  else if (local_PIR_state == true && radio_master_PIR_state == true)
  {
    leds[0] = CRGB(255, 0, 0); //green
    FastLED.show();
  }
}

void local_switch_PIR_on_off()
{
  if (digitalRead(pir_on_off_button) == LOW)
  {
    local_PIR_state = true;
    digitalWrite(button_LED, HIGH);
  }
  else
  {
    local_PIR_state = false;
    digitalWrite(button_LED, LOW);
  }
}

void sense_and_report()
{
  if (radio_master_PIR_state == true && local_PIR_state == true)
  {
    if (digitalRead(pir_pin) == HIGH && motion_timer == false)
    {
      Serial.println("MOTION SENSED!");
      myRadio.stopListening();
      myRadio.openWritingPipe(addresses[4]);
      for (int i = 0; i < 50; i++)
      {
        myRadio.write(&light_ON_number_message, sizeof(light_ON_number_message));
      }
      timer_state = false;
      myRadio.startListening();
      // delay(7000);
      motion_timer = true;
      motion_start_millis = millis();
    }
    if (motion_timer == true && millis() >= motion_start_millis + motion_debounce_time)
    {
      motion_timer = false;
    }
    if (digitalRead(pir_pin) == LOW && timer_state == false)
    {
      timer_state = true;
      start_millis = millis();
    }
    if (millis() >= timer_maximum + start_millis && timer_state == true)
    {
      timer_state = false;
      myRadio.stopListening();
      myRadio.openWritingPipe(addresses[4]);
      for (int i = 0; i < 50; i++)
      {
        myRadio.write(&light_OFF_number_message, sizeof(light_OFF_number_message));
      }
      myRadio.startListening();
      delay(1000);
    }
  }
}

void debug()
{
  Serial.print("RADIO MASTER STATE:  ");
  Serial.println(radio_master_PIR_state);
  Serial.println();
  Serial.println(noMotion);
}
