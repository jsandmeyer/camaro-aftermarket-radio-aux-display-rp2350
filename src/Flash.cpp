#include <EEPROM.h>

#include "Flash.h"

Flash::Flash() {

    EEPROM.begin(256);

    if (EEPROM.read(0) != 0x00 || EEPROM.read(1) != 0xFF) {
        // erase
        for (int i = 0; i < 256; i++) {
            EEPROM.write(i, 0x00);
        }

        EEPROM.write(1, 0xFF);
        EEPROM.write(UNITS, UNITS_DEFAULT);
    }

    units = EEPROM.read(UNITS);

    EEPROM.end();
}

void Flash::saveUnits(const uint8_t newUnits) {
    EEPROM.begin(256);
    EEPROM.write(UNITS, newUnits);
    EEPROM.end();

    units = newUnits;
}

uint8_t Flash::getUnits() const {
    return units;
}

