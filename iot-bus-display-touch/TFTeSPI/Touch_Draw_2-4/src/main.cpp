/*
  This is a simple paint sketch!

  Tested with IoT-Bus 2.4" QVGA TFT Display of 320 x 240 pixels and touchscreen

  Uses font 2 only

  The example will read the current state of the touch screen and output it to the serial
  port as mapped X and Y coordinates.

  Created by Rowboteer: 22/11/15
*/

// include touchscreen library
#include <XPT2046_Touchscreen.h>

// Call up the TFT driver library
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

// Invoke custom TFT driver library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// These pins are defined in User_Setup.h and have already been setup to be correct for the IoT-Bus Display

//#define TFT_MISO 19
//#define TFT_MOSI 23
//#define TFT_SCLK 18
//#define TFT_CS    5  // Chip select control pin
//#define TFT_DC    27  // Data Command control pin
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

/* Create an instance of the touch screen library */
#define CS_PIN  16

XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts

int color = TFT_WHITE;     //Starting paint brush color

// Palette button colour sequence
unsigned int colors[10] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_BLACK, TFT_CYAN, TFT_YELLOW, TFT_WHITE, TFT_MAGENTA, TFT_BLACK, TFT_BLACK};

// touchscreen  calibration values
#define X_MIN 256
#define X_MAX 3632
#define Y_MIN 274
#define Y_MAX 3579

int swatchWidth;
int swatchHeight;

void setup()
{
  Serial.begin(115200);

  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);

  ts.begin();
  ts.setCalibration(X_MIN, X_MAX, Y_MIN, Y_MAX);

  tft.init();
  
  // Set the TFT and touch screen to landscape orientation
  tft.setRotation(3);
  ts.setRotation(3);

  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);

  swatchWidth = ts.getWidth()/10;
  swatchHeight = 34;
 
  //Draw the palette
  for (int i = 0; i < 10; i++)
  {
    tft.fillRect(i * swatchWidth, 0, swatchWidth, swatchHeight, colors[i]);
  }

  //Draw the clear screen button
  tft.setCursor(ts.getWidth()-swatchWidth*1.5, 3, 2); // x,y,font
  tft.setTextColor(TFT_WHITE);
  tft.print("Clear");
  tft.drawRect(0, 0, ts.getWidth()-1, swatchHeight, TFT_WHITE);

  // Plot the current colour in the screen clear box
  tft.fillRect(ts.getWidth() - swatchWidth, 20, 12, 12, color);
}

/* Main program */
void loop()
{
  // Check if the touch screen is currently pressed
  // Raw and coordinate values are stored within library at this instant

  if (ts.touched()) 
  {
    Serial.println("touched");
    // Read the current X and Y axis as mapped co-ordinates at the last touch time

    TS_Point p = ts.getMappedPoint();

    // mapped pixel
    Serial.print(p.x); Serial.print(","); Serial.println(p.y);
    
    // Detect paint brush color change
    if (p.y < swatchHeight + 2)
    {
      if (p.x / swatchWidth > 7)
      {
        // Clear the screen to current color
        tft.fillRect(0, swatchHeight, ts.getWidth(), ts.getHeight()-1, color);
        Serial.println("clear screen to current color");
      }
      else
      {
        color = colors[p.x / swatchWidth];
        // Update the current color in the clear box
        tft.fillRect(ts.getWidth() - swatchWidth, 20, 12, 12, color);
        Serial.println("Update the current color in the clear box");
      }
    }
    else
    {
      // draw a point
      tft.fillCircle(p.x, p.y, 2, color);
      Serial.println("fillcircle");
    }
  }
}
