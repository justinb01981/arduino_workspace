
// Your sketch must #include this library, and the Wire library.
// (Wire is a standard library included with Arduino.):

#include <SFE_BMP180.h>
#include <Wire.h>

// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;

#define ALTITUDE 1655.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters

int pin_led1 = 14;
int pin_led2 = 12;
int pin_led3 = 13;
int pin_relay = 3; //rx
int pin_motionsensor = 15;//d8
int pin_motionsensor_state = 0;
int pin_relay_lifetime = 0;
int loop_interval_ms = 200;
int TRIGGERED_LIFETIME = 50; // "loops"
double temp_state = 0;
double temp_target = 22.2; // celcius
double temp_target_min = 12.77; // celcius
int temp_target_motion_lifetime_sec = 3600; // target temp when motion sensed
int temp_target_min_lifetime_sec = 300; // minimum temp
int counter =  0;
bool relay_high = false;

void setup()
{
  // init pins
  pinMode(pin_motionsensor, INPUT_PULLUP);
  pinMode(pin_relay, OUTPUT);
  pinMode(pin_led1, OUTPUT);
  pinMode(pin_led2, OUTPUT);
  pinMode(pin_led3, OUTPUT);
  
  
  //Serial.begin(9600);
  ///Serial.println("REBOOT");
}

void color_led(int state)
{
  digitalWrite(pin_led1, state == 1 || state == 3);
  digitalWrite(pin_led2, state == 0 || state == 3);
}

void relay_trigger_loop()
{
    // set relay switch to close
    if(pin_relay_lifetime > 0)
    {
      
      if(!relay_high) 
      {
        color_led(1);
        relay_high = true;
        digitalWrite(pin_relay, HIGH);
      }
      pin_relay_lifetime--;
    }
    else {


      // read from motion sensor
      pin_motionsensor_state = digitalRead(pin_motionsensor);

      if(pin_motionsensor_state)
      {
        static long countlast = 1;
        
        // motion detected, 
        pin_relay_lifetime = TRIGGERED_LIFETIME;
        //Serial.println("trig after N(cycles):");
        //Serial.println(counter-countlast);

        countlast = counter;
      }
      else 
      {
        
        
        if(relay_high) {
          color_led(2);
          digitalWrite(pin_relay, LOW);
          relay_high = false;
        }
      }


    }

  counter += 1;
  
}

void loop()
{
  
  relay_trigger_loop();

  delay(loop_interval_ms);  // Pause for
}
