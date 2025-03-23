#ifndef FLASH_H
#define FLASH_H

constexpr int UNITS = 2;
constexpr uint8_t UNITS_DEFAULT = 0;

class Flash {
    uint8_t units;
public:
    Flash();
    void saveUnits(uint8_t newUnits);
    uint8_t getUnits() const;
};

#endif //FLASH_H
