
// Your sketch must #include this library, and the Wire library.
// (Wire is a standard library included with Arduino.):

#include <SFE_BMP180.h>
#include <Wire.h>

// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;

#define ALTITUDE 1655.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters

int pin_led1 = 11;
int pin_led2 = 10;
int pin_led3 = 9;
int pin_relay = A1;
int pin_motionsensor = 7;
int pin_motionsensor_state = 0;
int pin_relay_lifetime = 0;
int loop_interval_ms = 5000;
double temp_state = 0;
double temp_target = 22.2; // celcius
double temp_target_min = 12.77; // celcius
int temp_target_motion_lifetime_sec = 3600; // target temp when motion sensed
int temp_target_min_lifetime_sec = 300; // minimum temp

void setup()
{
  // init pins
  pinMode(pin_motionsensor, INPUT);
  pinMode(pin_relay, OUTPUT);
  pinMode(pin_led1, OUTPUT);
  pinMode(pin_led2, OUTPUT);
  pinMode(pin_led3, OUTPUT);
  
  //
  Serial.begin(9600);
  Serial.println("REBOOT");

  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
}

void color_led(int state)
{
  digitalWrite(pin_led1, state == 1 || state == 3);
  digitalWrite(pin_led2, state == 0 || state == 3);
}

void thermostat_loop()
{
  int temp_below = 0;

  double temp_error_margin = 0.1;
  
  if(temp_target >= temp_state - temp_error_margin)
  {
    temp_below = 1;
  }
  
  // set relay switch to close
  if(pin_relay_lifetime > 0)
  {
    digitalWrite(pin_relay, temp_below);
    color_led(temp_below);
    
    pin_relay_lifetime--;
    if(pin_relay_lifetime == 0)
    {
      digitalWrite(pin_relay, 0);
      color_led(0);
    }
  }

  // read from motion sensor
  pin_motionsensor_state = digitalRead(pin_motionsensor);

  if(pin_motionsensor_state)
  {
    // motion detected, set target temperature
    pin_relay_lifetime = temp_target_motion_lifetime_sec / (loop_interval_ms / 1000);
  }

  if(temp_state < temp_target_min)
  {
    pin_relay_lifetime = temp_target_min_lifetime_sec;
  }
}

void loop()
{
  char status;
  double T,P,p0,a;
  
  thermostat_loop();

  // Loop here getting pressure readings every 10 seconds.

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:
  
  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");
  
  // If you want to measure altitude, and not pressure, you will instead need
  // to provide a known baseline pressure. This is shown at the end of the sketch.

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      temp_state = T;
      
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  delay(loop_interval_ms);  // Pause for 5 seconds.
}

