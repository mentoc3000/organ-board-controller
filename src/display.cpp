#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    // Display allocation failed
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OrganBoard");
  display.display();
  display.setTextSize(1);
  display.setCursor(0, 24);
  display.println("by JP Sheehan");
  display.display();
}

void displaySend(int cc, int channel, int midiValue) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("CC:      ");
  display.println(cc);
  display.print("Channel: ");
  display.println(channel);
  display.print("MIDI:    ");
  display.println(midiValue);
  display.display();
}

void displayCalibration() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Calibrating...");
  display.display();
}

void displayCalibratingKnob(int knobIndex) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Calibrating CC ");
  display.println(knobIndex);
  display.display();
}

void displayValue(const char *label, int value) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(label);
  display.print(": ");
  display.println(value);
  display.display();
}
