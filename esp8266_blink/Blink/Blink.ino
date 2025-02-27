/**
 * Blink for Node MCU
 * also see https://arduino.stackexchange.com/questions/38477/does-the-node-mcu-v3-lolin-not-have-a-builtin-led
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"

// On a NodeMCU board the built-in led is on GPIO pin 2
#define LED_BUILTIN 24    

void setup()
{
  Serial.begin(14400);

  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  Serial.println("Node MCU blink!");
  // turn the LED on (the built-in led on a Node MCU board is active low)
  digitalWrite(LED_BUILTIN, LOW);
  // wait for a second
  delay(20);
  // turn the LED off (the built-in led on a Node MCU board is active low)
  digitalWrite(LED_BUILTIN, HIGH);

   // wait for a second
  delay(1000);
}
