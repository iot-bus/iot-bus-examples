/*
  This is a simple paint sketch!

  Tested with a 2.4" ILI9341 based display of 320 x 240 pixels

  Uses font 2 only

  The example will read the current state of the touch screen and output it to the serial
  port as raw X and Y coordinates, as the current position in pixels and a zone number.

  >>>> YOU MUST CALIBRATE THE TOUCH SCREEN AND COPY THE SETTINGS TO LINE 48 OF THIS SKETCH <<<<

  >>>>        YOU MUST CHANGE THE PIN DEFINITION AT LINES 29-32 TO SUIT YOUR SETUP         <<<<

  Created by Rowboteer: 22/11/15
*/

#include <XPT2046_Touchscreen.h>

// Call up the TFT driver library
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

//#define SPI_SETTING    SPISettings(1000000, MSBFIRST, SPI_MODE0) // this is the max for the touchscreen controller
//#define SPI_FREQUENCY  40000000 // TFT SPI frequency - this is the max for ESP32 - the ILI9341 can work at this speed

// Invoke custom TFT driver library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

/* Create an instance of the touch screen library */
#define CS_PIN  16
#define TIRQ_PIN  17

// These are the pins used to interface between the 2046 touch controller and ESP32
//#define DOUT 19  /* Data out pin (T_DO) of touch screen */
//#define DIN  23  /* Data in pin (T_DIN) of touch screen */
//#define DCS  16  /* Chip select pin (T_CS) of touch screen */
//#define DCLK 18  /* Clock pin (T_CLK) of touch screen */

XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
// this was the old one
//XPT2046_Touchscreen touch(CS_PIN, TIRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling

//#include <TFT_Touch.h>
/* Create an instance of the touch screen library */
//TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT);

int ColorPaletteHigh = 30; // Height of palette boxes
int color = TFT_WHITE;     //Starting paint brush color

// Pallete button colour sequence
unsigned int colors[10] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_BLACK, TFT_CYAN, TFT_YELLOW, TFT_WHITE, TFT_MAGENTA, TFT_BLACK, TFT_BLACK};

void setup()
{
  Serial.begin(115200);

  ts.begin();
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  tft.init();

  //This is the calibration line produced by the TFT_Touch_Calibrate_v2 sketch
  //touch.setCal(256, 3632, 274, 3579, 320, 240, 1);
  
  // Set the TFT and touch screen to landscape orientation
  tft.setRotation(1);
  ts.setRotation(4);

  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);

  //Draw the pallete
  for (int i = 0; i < 10; i++)
  {
    tft.fillRect(i * 32, 0, 32, ColorPaletteHigh, colors[i]);
  }

  //Draw the clear screen button
  tft.setCursor(264, 7, 2); // x,y,font
  tft.setTextColor(TFT_WHITE);
  tft.print("Clear");
  tft.drawRect(0, 0, 319, 30, TFT_WHITE);

  // Plot the current colour in the screen clear box
  tft.fillRect(300, 9, 12, 12, color);
}


// calibrate touchscreen  
#define X_LEFT 256
#define X_DISTANCE 3632
#define Y_BOTTOM 274
#define Y_DISTANCE 3579

/* Main program */
void loop()
{
  int X_Coord;
  int Y_Coord;

  // Check if the touch screen is currently pressed
  // Raw and coordinate values are stored within library at this instant
  // for later retrieval by GetRaw and GetCoord functions.
  //if (touch.Pressed()) 
  if (ts.touched()) 
  {
    Serial.println("touched");
    // Read the current X and Y axis as co-ordinates at the last touch time
    // The values were captured when Pressed() was called!
    TS_Point p = ts.getPoint();

    // Translate raw touch data to tft pixel (320x240 display)
    X_Coord = (p.x-X_LEFT)/(X_DISTANCE/320);
    Y_Coord = 240-((p.y-Y_BOTTOM)/(Y_DISTANCE/240)); // allow for screen to be rotated 180 degrees
//    X_Coord = touch.x;
//    Y_Coord = touch.y;

    // raw data
    //Serial.print(p.x); Serial.print(","); Serial.println(p.y);
    // translated pixel
    Serial.print(X_Coord); Serial.print(","); Serial.println(Y_Coord);
    
    // Detect  paint brush color change
    if (Y_Coord < ColorPaletteHigh + 2)
    {
      if (X_Coord / 32 > 7)
      {
        // Clear the screen to current colour
        tft.fillRect(0, 30, 399, 239, color);
        Serial.println("clear screen to current color");
      }
      else
      {
        color = colors[X_Coord / 32];
        // Update the current colour in the clear box
        tft.fillRect(300, 9, 12, 12, color);
         Serial.println("Update the current colour in the clear box");
      }
    }
    else
    {
      tft.fillCircle(X_Coord, Y_Coord, 2, color);
      Serial.println("fillcircle");
    }
  }
}
