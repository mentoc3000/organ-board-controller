#include <EEPROM.h>

void setupStorage() {}

void saveCalibrationData(int knobIndex, int rawMin, int rawMax) {
  // Save calibration data to storage
  const int baseAddress = knobIndex * 4; // 2 bytes each for min and max
  EEPROM.put(baseAddress, rawMin);
  EEPROM.put(baseAddress + 2, rawMax);
}

bool loadCalibrationData(int knobIndex, int &rawMin, int &rawMax) {
  // Load calibration data from storage
  const int baseAddress = knobIndex * 4; // 2 bytes each for min and max
  EEPROM.get(baseAddress, rawMin);
  EEPROM.get(baseAddress + 2, rawMax);
  return true; // Return true if successful
}
