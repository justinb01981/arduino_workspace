/*
 Fade 3
 
 This example shows how to fade an LED on pin 9
 using the analogWrite() function.
 
 This example code is in the public domain.
 */

 // justin / 7-30-2023 -- lilypad remeber to use the USB board variant when choosing

 
const int HI = 128;

int led[3] = {A0,A1,A2};           // the pin that the LED is attached to
int brightness_a[3] = {0, HI/3, HI/2};    // how bright the LED is
int fadeAmount_a[3] = {2,3,4};    // how many points to fade the LED by
int l = 1;
int m[] = {1, 1, 1};
int h = HI;

// the setup routine runs once when you press reset:
void setup()  { 
  // declare pin 9 to be an output:
  for(int i = 0; i < sizeof(led)/sizeof(int); i++) { pinMode(led[i], OUTPUT); }
} 

// the loop routine runs over and over again forever:
void loop()  { 
  for(int i = 0; i < 3; i++) {
    int *brightness = &brightness_a[i];
    int *fadeAmount = &fadeAmount_a[i];
    
    // set the brightness of pin 9:
    analogWrite(led[i], *brightness);    
  
    // change the brightness for next time through the loop:
    *brightness = *brightness + (*fadeAmount / m[i]);
  
    // reverse the direction of the fading at the ends of the fade: 
    if (*brightness <= l || *brightness >= h/m[i]) {
      *fadeAmount = -*fadeAmount ; 
    }     
    //Serial.print("again\n");
  }
  // wait for 30 milliseconds to see the dimming effect    
  delay(30);                            
}
