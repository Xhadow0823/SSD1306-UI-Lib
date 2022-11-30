/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <Arduino.h>
// #include <SPI.h>
#include <Wire.h>
// #include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ISRs.h>
#include <agents.h>
#include <multiApp.h>

#include <BMI160Gen.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void readEncoder();
void swClick();

// Demo0 demo0(display);
// Dice demo0(display);
// __UIHelper UIHelper0(display);

void setup() {
  Serial.begin(9600);

  // free memory checking...
  // #define testSize (64 * 2)
  // char * buffer = (char*)malloc(testSize * sizeof(char));  // 128 good, 192 bad
  // if (!buffer) {
  //   Serial.println(F("malloc failed"));
  // }


//   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
// /*
//   // Show initial display buffer contents on the screen --
//   // the library initializes this with an Adafruit splash screen.
//   display.display();
//   delay(2000); // Pause for 2 seconds

//   // Clear the buffer
//   display.clearDisplay();

//   // Draw a single pixel in white
//   display.drawPixel(10, 10, SSD1306_WHITE);

//   // Show the display buffer on the screen. You MUST call display() after
//   // drawing commands to make them visible on screen!
//   display.display();
//   delay(2000);
//   // display.display() is NOT necessary after every single drawing command,
//   // unless that's what you want...rather, you can batch up a bunch of
//   // drawing operations and then update the screen all at once by calling
//   // display.display(). These examples demonstrate both approaches...

//   testdrawline();      // Draw many lines

//   testdrawrect();      // Draw rectangles (outlines)

//   testfillrect();      // Draw rectangles (filled)

//   testdrawcircle();    // Draw circles (outlines)

//   testfillcircle();    // Draw circles (filled)

//   testdrawroundrect(); // Draw rounded rectangles (outlines)

//   testfillroundrect(); // Draw rounded rectangles (filled)

//   testdrawtriangle();  // Draw triangles (outlines)

//   testfilltriangle();  // Draw triangles (filled)

//   testdrawchar();      // Draw characters of the default font

//   testdrawstyles();    // Draw 'stylized' characters

//   testscrolltext();    // Draw scrolling text

//   testdrawbitmap();    // Draw a small bitmap image

//   // Invert and restore display, pausing in-between
//   display.invertDisplay(true);
//   delay(1000);
//   display.invertDisplay(false);
//   delay(1000);

//   testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps

// */
//   #define CLK 2
//   #define DT 3
//   pinMode(CLK, INPUT);
//   pinMode(DT, INPUT);

//   attachInterrupt(0, readEncoder, CHANGE);
//   attachInterrupt(1, swClick, CHANGE);

//   //set timer2 interrupt at 1kHz
//   cli();
//   TCCR2A = 0;  // set entire TCCR2A register to 0
//   TCCR2B = 0;  // same for TCCR2B
//   TCNT2  = 0;  //initialize counter value to 0
//   OCR2A = 249;  // = (16*10^6) / (64*1000) - 1 (must be <256)
//   TCCR2A |= (1 << WGM21);  // turn on CTC mode
//   TCCR2B |= (1 << CS22);
//   TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
//   sei();

  display.drawFastVLine(display.width() / 2, 0, 32, WHITE);
  display.display();

//   demo0.UIHelper = &UIHelper0;
//   demo0.setup();

// =============GYRO=====================
  BMI160.begin(BMI160GenClass::SPI_MODE, 10);
  uint8_t dev_id = BMI160.getDeviceID();
  Serial.print("DEVICE ID: ");
  Serial.println(dev_id, HEX);
  // BMI160.setGyroRange(250);
  BMI160.setAccelerometerRange(250);
  Serial.println("Initializing IMU device...done.");

  Serial.print("free space: ");
  Serial.println(calcFreeMemorySpaceSize());
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767

  float g = (gRaw * 250.0) / 32768.0;
  return g;
}


int gxRaw, gyRaw, gzRaw;         // raw gyro values
float gx, gy, gz;

void loop() {
  // SWAgent.clearClicked();
  // SWAgent.update();
  // REAgent.clearOffset();
  // REAgent.update();

  // display.clearDisplay();


// ==================GYRO=====================
  BMI160.readGyro(gxRaw, gyRaw, gzRaw);
  BMI160.readAccelerometer(gxRaw, gyRaw, gzRaw);
  gx = convertRawGyro(gxRaw);
  gy = convertRawGyro(gyRaw);
  gz = convertRawGyro(gzRaw);
  // static char gyroInfoBuffer[128] = "";
  // snprintf(gyroInfoBuffer, 128, "gx=%4f, gy=%4f, gz=%4f", gx, gy, gz);
  // // Serial.println(gyroInfoBuffer);
  display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(display.width()/2-(12*3)/2, display.height()/2-(8*3)/2);
    display.println((gx>gy?(gx>gz?"gx":"gz"):(gy>gz?"gy":"gz")));

    int x = 70, y = sin((gy)/(16) / (PI/2)) * 70;
    display.drawLine(display.width()/2 - x, display.height()/2 - y, display.width()/2 + x, display.height()/2 + y, SSD1306_WHITE);
    display.drawFastVLine(( (gx+17) * display.width() / (17*2)), 2, 20, WHITE);
    display.drawFastVLine(( (gy+17) * display.width() / (17*2)), 22, 20, WHITE);
    display.drawFastVLine(( (gz+17) * display.width() / (17*2)), 44, 20, WHITE);
  display.display();

  // Serial.print(gx);
  // Serial.print(", ");
  // Serial.print(gy);
  // Serial.print(", ");
  // Serial.print(gz);
  // Serial.println();
// ==================GYRO END=====================


  // demo0.loop();
  // // UIHelper0.openMenu();
  
  
  // display.display();

  delay(100);
}