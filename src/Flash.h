#ifndef FLASH_H
#define FLASH_H

class Flash {
    static bool beginCritical();
    static void endCritical();
    static bool isSetUp();
public:
    static void setDefaults();
    static void saveUnits(uint8_t newUnits);
    [[nodiscard]] static uint8_t getUnits() ;
};

#endif //FLASH_H
