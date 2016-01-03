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
struct RGB displayIMG[8][8] = {0};
struct RGB bgColor = teal;
struct RGB fgColor = white;
int lumin = 20;
int pixelPos = 0;
int text_delay = 60;

struct RGB hex2RGB(long hexValue)
{
  struct RGB rgbColor;
  rgbColor.r = hexValue >> 16;         // Extract the RR byte
  rgbColor.g = hexValue >> 8 & 0xFF;   // Extract the GG byte
  rgbColor.b = hexValue & 0xFF;        // Extract the BB byte

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

void drawFast(RGB pic[8][8]) {
  for(int r = 0; r < 8; r++) {
    for(int c = 0; c < 8; c++) {
      RGB pRGB = pic[r][c];
      matrix.drawPixel(c, r, matrix.Color(pRGB.r, pRGB.g, pRGB.b));
    }
  }
  matrix.show();
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
    delay(text_delay);
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
    if (inputType == String("i\n") && inChar == ' ') {
      displayIMG[pixelPos / 8][pixelPos % 8] = hex2RGB(strtol(inputString.c_str(), NULL, 0));
      pixelPos++;
      inputString = "";
    }
    else {
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag
      // so the main loop can do something about it:
      if (inChar == '\n') {
        stringComplete = true;
        if (inputType == String("i")) pixelPos = 0;
      }
    }
  }
}

void setup() {
  // initialize serial:
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // LED matrix setup
  matrix.begin();
  matrix.setBrightness(lumin);
  matrix.setTextColor( matrix.Color(fgColor.r, fgColor.g, fgColor.b) );
  matrix.setTextWrap(false);
  // hold everythng until serial stream is open, doing nothing:
  while (!Serial);
  scrollText("Ready!\n");
  Serial.print("Ready!\n");
}

void loop() {
  serialEvent(); // call the function
  // print the string when a newline arrives:
  if (stringComplete) {
    // TODO: Allow querying of current variable values
    if (inputType == "") {
      // collect input type
      inputType = inputString;
      Serial.print(inputType);
    }
    else {
      // do assorted things based on input type
      if (inputType == String("t\n")) {
        // show the text
        scrollText(inputString);
        Serial.print("text printed\n");
      }
      else if (inputType == String("b\n")) {
        // set text background color
        bgColor = hex2RGB( strtol(inputString.c_str(), NULL, 0) );
        Serial.print("background color changed: r[");
        Serial.print(bgColor.r);
        Serial.print("] g[");
        Serial.print(bgColor.g);
        Serial.print("] b[");
        Serial.print(bgColor.b);
        Serial.print("]\n");
      }
      else if (inputType == String("f\n")) {
        // set foreground color and push to text color
        fgColor = hex2RGB( strtol(inputString.c_str(), NULL, 0) );
        matrix.setTextColor( matrix.Color(fgColor.r, fgColor.g, fgColor.b) );
        Serial.print("foreground color changed: r[");
        Serial.print(fgColor.r);
        Serial.print("] g[");
        Serial.print(fgColor.g);
        Serial.print("] b[");
        Serial.print(fgColor.b);
        Serial.print("]\n");
      }
      else if (inputType == String("i\n")) {
        drawFast(displayIMG);
        Serial.print("image displayed\n");
      }
      else if (inputType == String("l\n")) {
        lumin = (int)(strtol(inputString.c_str(), NULL, 0));
        matrix.setBrightness(lumin);
        Serial.print("brightness set to: ");
        Serial.print(lumin);
        Serial.print("\n");
      }
      else if (inputType == String("s\n")) {
        text_delay = (int)(strtol(inputString.c_str(), NULL, 0));
        Serial.print("text delay set to: ");
        Serial.print(text_delay);
        Serial.print("\n");
      }
      // set inputType to blank
      inputType = "";
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}




