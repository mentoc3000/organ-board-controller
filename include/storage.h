#pragma once

void setupStorage();
void saveCalibrationData(int knobIndex, int rawMin, int rawMax);
bool loadCalibrationData(int knobIndex, int &rawMin, int &rawMax);
