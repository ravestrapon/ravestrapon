#ifndef SAVEDSETTINGS_H
#define SAVEDSETTINGS_H

#include <EEPROM.h>

// Global variables defined in the main source file that this module needs access to
extern int current_brightness;
extern volatile int brightness_setting;

// EEPROM addresses/values for various values
constexpr int kBrightnessAddr = 0;
constexpr int kBrightnessValidAddr = 1;
constexpr int kBrightnessValidMagicNum = 0xF0;

// Take the current brightness setting and save it to the EEPROM, this
// also writes a magic number in another address that indicates that the
// stored brightness value is valid.
void saveBrightnessToEEPROM() {
  EEPROM.write(kBrightnessAddr, current_brightness);
  EEPROM.write(kBrightnessValidAddr, kBrightnessValidMagicNum);
}

// Check through the EEPROM to see if any settings were stored on an earlier
// run.  If so, respore them.
void restoreEEPROMSavedSettings() {
  bool is_saved_brightness_valid = (EEPROM.read(kBrightnessValidAddr) ==
                                    kBrightnessValidMagicNum);
  if (is_saved_brightness_valid) {
    current_brightness = brightness_setting = EEPROM.read(kBrightnessAddr);
  } else {
    saveBrightnessToEEPROM();
  }
}


#endif  // SAVEDSETTINGS_H
