#pragma once
#include <Adafruit_SSD1306.h>

// Display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// I2C address for most SSD1306 128x32 displays
#define OLED_ADDR 0x3C

extern Adafruit_SSD1306 display;

void setupDisplay();
void displaySend(int analogValue, int channel, int midiValue);
void displayCalibration();
void displayValue(const char *label, int value);
