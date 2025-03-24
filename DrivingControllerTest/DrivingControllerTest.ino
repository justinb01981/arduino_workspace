// Program used to test the driving simulator functions on 
// the USB Joystick object on the Arduino Leonardo or 
// Arduino Micro.
//
// Matthew Heironimus
// 2016-05-29   Original version.
//------------------------------------------------------------

// 2025-02-23 justin brady hacking up to test gyroscope input
#include <SPI.h>
#include <Wire.h>
#include <Arduino_LSM6DSOX.h> 
#include <limits.h>

#include "Joystick.h"

const unsigned int RANGE_STEER = 600; // my encoder has 600pt resolution
const unsigned int RANGE_ACCEL = 300;
const unsigned int DIV = 6;

/* encoder variables */
const int encoder_a = PIND0+2; // Green - pin - Digital
const int encoder_b = PIND0+3; // White - pin - Digital
long encoder = 0, cnt = 1;
char prv;

void encoderOnChange(char prv, char cur) {
  if(prv == cur) { cnt = cnt * -1; }
  else { encoder += cnt; }


}

/* encoder int handlers */
void encoderPinChangeA() {
  encoderOnChange(prv, 'a');
  prv = 'a';
}

void encoderPinChangeB() {
  encoderOnChange(prv, 'b');
  prv = 'b';
}

bool encoderInit(void) {
  pinMode(encoder_a, INPUT_PULLUP);
  pinMode(encoder_b, INPUT_PULLUP);

  attachInterrupt(0, encoderPinChangeA, CHANGE);
  attachInterrupt(1, encoderPinChangeB, CHANGE);
  return true;
}

bool encoderReady(void) {
  return true;
}

/* joystick variables */

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, 4, 0,
  false, false, false, false, false, false,
  false, false, true, true, true);

const unsigned long gcButtonDelta = /*500*/ 5;

unsigned int PIN_btn0 = PIND0+1;

unsigned long gNextTime = 0;
float xO = 0, yO = 0, zO = 0;

float min[] = {INT_MAX, INT_MAX, INT_MAX};
float max[] = {0, 0, 0};
boolean btn_press [] = {false, false, false};

// moved to global scope to save between loops
float x, y, z, tmp;
float *pV = &x;

// filter table
const int FILTERLEN = 1;
float ftable[FILTERLEN];
int ftableOff;


void testSingleButtonPush(unsigned int button)
{
  if (button > 0)
  {
    Joystick.releaseButton(button - 1);
  }
  if (button < 4)
  {
    Joystick.pressButton(button);
  }
}

void zeroIMU()
{
  int i;

  xO = yO = zO = 0;
  
  for(i = 0; i < sizeof(min)/sizeof(float); i++)
  {
    min[i] = INT_MAX;
    max[i] = 0;
  }

}

void save(float x, unsigned idx) {
  if(x > max[idx]) max[idx] = x;
  if(x < min[idx]) min[idx] = x;
}

void callResetEncoder(void) {
  {
    Serial.println("reset encoder");

    cnt *= -1;
    encoder = RANGE_STEER / 2;
  } 
}

void setup() {

  Serial.begin(9600);

  if(!encoderInit()) {
    Serial.println("encoder init failed");
  }

  // encoder takes 2 interrupts
  pinMode(PIN_btn0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_btn0), callResetEncoder, CHANGE);
  
  Joystick.begin(false);

  Joystick.setAcceleratorRange(0, RANGE_ACCEL);
  Joystick.setBrakeRange(0, RANGE_ACCEL);
  Joystick.setSteeringRange(0, RANGE_STEER);
  
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  

  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {

  if(encoderReady())
  {
    unsigned k;
    long* pV = &encoder, tmp;

    //Serial.println(*pV);

    ftable[ftableOff] = *pV;
    save(*pV, k); // min/max tracking

    ftableOff += 1;
    if(ftableOff >= FILTERLEN) 
    {
      ftableOff = 0;
    }
    
    tmp = 0;
    for(int i = 0; i < FILTERLEN; i++)  tmp += ftable[i];
    tmp /= FILTERLEN;

    // calculate steering from encoder range to joystick range
    k = 0;
    float S = tmp/DIV;
    if(S < 0) S = 0;
    if(S > RANGE_STEER) S = RANGE_STEER;
    Joystick.setSteering(S);

    // calculate accel
    k = 1;
    unsigned int Sacc = analogRead(A2);
    float sensorValueAc;
    save(Sacc, k);
    sensorValueAc = (Sacc-min[k]) / (max[k]-min[k]) * RANGE_ACCEL; // read range 172 - 730
    Joystick.setAccelerator(sensorValueAc);

    // calculate brake
    k = 2;
    unsigned int Sbr = analogRead(A1);
    float sensorValueBr;
    save(Sbr, k);
    sensorValueBr = (Sbr-min[k]) / (max[k]-min[k]) * RANGE_ACCEL; // read range 172 - 730
    Joystick.setBrake(sensorValueBr);
    //Serial.println(sensorValueBr);

  }
  
  //  update joystick state
  if (millis() >= gNextTime)
  {
    gNextTime = millis() + gcButtonDelta;

    Joystick.sendState();    

  }
}

