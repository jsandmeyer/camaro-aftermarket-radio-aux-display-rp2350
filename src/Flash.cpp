#include <SerialUSB.h>
#include <hardware/flash.h>

#include "Flash.h"
#include "Debug.h"

extern "C" uint8_t _EEPROM_start; // NOLINT(*-reserved-identifier)

static uint8_t* eepromData = &_EEPROM_start;
static intptr_t eepromWriteOffset = reinterpret_cast<intptr_t>(eepromData) - static_cast<intptr_t>(XIP_BASE);

static constexpr uint8_t header[4] = {0x01, 0xCC, 0x10, 0xF7};

// note index must start at 4 to leave room for header
static constexpr size_t UNITS_INDEX = 4;
static constexpr uint8_t UNITS_DEFAULT = 0;

void Flash::beginCritical() {
    noInterrupts();
    rp2040.idleOtherCore();
}

void Flash::endCritical() {
    rp2040.resumeOtherCore();
    interrupts();
}

bool Flash::isSetUp() {
    for (size_t i = 0; i < std::size(header); i++) {
        if (eepromData[i] != header[i]) {
            return false;
        }
    }

    return true;
}

void Flash::setDefaults() {
    if (!isSetUp()) {
        uint8_t _data[FLASH_PAGE_SIZE] = {};
        memcpy(_data, header, std::size(header));

        _data[UNITS_INDEX] = UNITS_DEFAULT;

        beginCritical();
        flash_range_erase(eepromWriteOffset, FLASH_SECTOR_SIZE);
        flash_range_program(eepromWriteOffset, _data, FLASH_PAGE_SIZE);
        endCritical();
    }

    DEBUG(Serial.printf("Flash() units=%x\n", getUnits()));
}

void Flash::saveUnits(const uint8_t newUnits) {
    uint8_t _data[FLASH_PAGE_SIZE] = {};
    memcpy(_data, eepromData, FLASH_PAGE_SIZE);
    _data[UNITS_INDEX] = newUnits;

    beginCritical();
    flash_range_erase(eepromWriteOffset, FLASH_SECTOR_SIZE);
    flash_range_program(eepromWriteOffset, _data, FLASH_PAGE_SIZE);
    endCritical();

    DEBUG(Serial.printf("saveUnits() units=%x\n", getUnits()));
}

uint8_t Flash::getUnits() {
    return eepromData[UNITS_INDEX];
}

