#include <EEPROM.h>

void setupStorage() {}

void saveCalibrationData(int pin, int rawMin, int rawMax) {
  // Save calibration data to storage
  const int baseAddress = pin * 4; // 2 bytes each for min and max
  int16_t min16 = rawMin;
  int16_t max16 = rawMax;
  EEPROM.put(baseAddress, min16);
  EEPROM.put(baseAddress + 2, max16);
}

bool loadCalibrationData(int pin, int &rawMin, int &rawMax) {
  // Load calibration data from storage
  const int baseAddress = pin * 4; // 2 bytes each for min and max
  int16_t min16, max16;
  EEPROM.get(baseAddress, min16);
  EEPROM.get(baseAddress + 2, max16);
  rawMin = min16;
  rawMax = max16;
  return true; // Return true if successful
}
