#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    // Display allocation failed
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Organ Board");
  display.display();
}

void updateDisplay(int analogValue, int midiValue) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Analog: ");
  display.println(analogValue);
  display.print("MIDI:   ");
  display.println(midiValue);
  display.display();
}
