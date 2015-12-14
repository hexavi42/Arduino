#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <stdlib.h>
#include "RGB.h"

#define PIN 1

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

String inputString = "";         // a string to hold incoming data
String inputType = "";           // a string to indicate type of incoming data
boolean stringComplete = false;  // whether the string is complete
int displayIMG[8][8] = []
struct RGB bgColor = teal
struct RGB fgColor = white
int pixelPos = 0

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // LED matrix setup
  matrix.begin();
  matrix.setBrightness(20);
  matrix.setTextColor( matrix.Color(fgColor.r, fgColor.g, fgColor.b) );
  matrix.setTextWrap(false);
  
}

void loop() {
  serialEvent(); //call the function
  // print the string when a newline arrives:
  if (stringComplete) {
    if (inputType == "") {
      // collect input type
      inputType = inputString
    }
    else {
      // do assorted things based on input type
      if (inputType == 't') {
        // show the text
        scrollText(inputString)
      }
      else if (inputType == 'f') {
        // set text background color
        bgColor = hex2RGB(strtol(inputString));
      }
      else if (inputType == 'b') {
        // set foreground color and push to text color
        fgColor = hex2RGB(strtol(inputString))
        matrix.setTextColor( matrix.Color(fgColor.r, fgColor.g, fgColor.b) );
      }
      else if (inputType == 'i') {
        drawFast(displayIMG)
      }
      // set inputType to blank
      inputType = ""
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
SerialEvent occurs whenever a new data comes in the
hardware serial RX. This routine is run between each
time loop() runs, so using delay inside loop can delay
response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inputType == 'i' && inChar == ' ') {
      displayIMG[pixelPos / 8][pixelPos % 8] = (int)strtol(inputString);
      pixelPos++;
      inputString = ""
    }
    else {
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag
      // so the main loop can do something about it:
      if (inChar == '\n') {
        stringComplete = true;
        if (inputType = 'i') pixelPos = 0
      }
    }
  }
}

struct RGB hex2RGB(int hexValue)
{
  struct RGB rgbColor;
  rgbColor.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
  rgbColor.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
  rgbColor.b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte

  return rgbColor; 
}

// Fill the dots one after the other with a color
void colorWipe(RGB color, uint8_t wait) {
  for(uint16_t row=0; row < 8; row++) {
    for(uint16_t column=0; column < 8; column++) {
      matrix.drawPixel(column, row, matrix.Color(color.r, color.g, color.b));
      matrix.show();
      delay(wait);
    }
  }
}

void drawFast(int pic[8][8]) {
  for(int row = 0; row < 8; row++) {
    for(int column = 0; column < 8; column++) {
      pRGB = hex2RGB(pic[column][row])
      matrix.drawPixel(column, row, matrix.Color(pRGB.r, pRGB.g, pRGB.b));
      matrix.show();
    }
  }
}

void scrollText(String textToDisplay) {
  int x = matrix.width();
  int pixelsInText = textToDisplay.length() * 7;
  
  matrix.setCursor(x, 0);
  matrix.print(textToDisplay);
  matrix.show();
  
  while(x > (matrix.width() - pixelsInText)) {
    matrix.fillScreen(matrix.Color(bgColor.r, bgColor.g, bgColor.b));
    matrix.setCursor(--x, 0);
    matrix.print(textToDisplay);
    matrix.show();
    delay(60);
  }
}




