/*
  stolen from:
  Arduino LSM6DSOX - Simple Accelerometer

  created 10 May 2021
  by Arturo Guadalupi

  This example code is in the public domain.
*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino_LSM6DSOX.h> 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/// end of lcd globals

// constants
float G = 32.1740485564; // ft/sec ^2
float Gmet = 9.80665; // meter/sec ^2
float Vbase = 0, Vbx, Vby, Vbz;

// spoofing Serial
#define Serial Serialc
class SerialC {
  public:
  void begin(int) {};
  void print(const float c) { display.print(c); };
  void println(const float s) { display.print(s); };
  void println(const char s[]) { display.print(s); };
  void println(void) { display.write(""); };

  bool operator!() { return false; }
};
SerialC Serialc;


const int HI = 128;

// Justin 2-6-25 converting to esp32 pins? see bootloader remapping for nano esp32 
int led[] = {A0,A1,A2};           // the pin that the LED is attached to
int i2cgpio[] = {SDA, SCL};        // a4,a5 on arduino = 11,12 on esp32

int brightness_a[3] = {0, HI/3, HI/2};    // how bright the LED is
int fadeAmount_a[3] = {2,3,4};    // how many points to fade the LED by
int l = 1;
//int m[] = {1, 1, 1};
int h = HI;


// functions


void setupDisplay() {

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.display();      // Show initial text
  delay(100);

}

void barfDisplay(const char* c)
{
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("barf"));
  display.println(c);
  display.display();      // Show initial text
  delay(100);
}

void setup() {

  Serial.begin(115200); // always first

  Wire.begin();//Wire.begin(i2cgpio[0], i2cgpio[1]); // init i2c on these pins, leaving @ default is bad idea when mixing boards

  setupDisplay();
  
  // declare pin 9 to be an output:
  for(int i = 0; i < sizeof(led)/sizeof(int); i++) { pinMode(led[i], OUTPUT); }

  // init imu sensor
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  // Serial.println("Accelerometer sample rate = ");
  // Serial.print(IMU.accelerationSampleRate());
  // Serial.println(" Hz");
  // Serial.println();
  // Serial.println("Acceleration in g's");
  // Serial.println("X\tY\tZ");

  
}

void brightenLed(float s) {
   s = s*33.0;
   float R[] = {255,128,64,8,0, 0, 0, 0, 0};
   float G[] = {0,0,0,0,0, 8,64, 128, 255};
   float B[] = {0,0,0,0, 0,  0, 0,0, 0};
   const int len = 9;

   int k = (len/2.0 + s);
   if(k < 0) k = 0;
   if(k >= len) k = len-1;

   analogWrite(led[0], R[k]);
   analogWrite(led[1], G[k]);
   analogWrite(led[2], B[k]);
}

void loop() {
  float x, y, z;

  if (IMU.accelerationAvailable()
    && IMU.readAcceleration(x, y, z) )
  {
    display.clearDisplay();
    display.setCursor(10, 0);
    
    float Dx = x-Vbx, Dy = y-Vby, Dz = z-Vbz;
    float Vft = Dx * G;

    brightenLed(Dx);

    Dx *= 100;
    Dy *= 100; 
    Dz *= 100;
    Serial.print(Dx);
    Serial.println("\nG/100 sec!");
    Serial.println("\n\t\t\tbuild 0.12(" __TIMESTAMP__")");

    if(Vbase == 0) 
    {
      Vbase = Vft;
      Vbx = x; Vby = y; Vbz = z;
    }
  }

  display.display();

  delay(66);
}
