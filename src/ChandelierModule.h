#ifndef CHANDELIERMODULE_H
#define CHANDELIERMODULE_H

#include "Timeout.h"
#include <vector>

using namespace std;

class ChandelierModule
{

public:
    int const ID;
    int const DATA_PIN;
    int const CLOCK_PIN;
    size_t const NUM_LIGHTS = 8;

    ChandelierModule(int id, int dataPin, int clockPin);

    void Update();
    bool HasAnyLightsOff();
    bool HasAllLightsOff();
    void SetOneLight();
    uint32_t GetLightStatus(int idx);
    void SetIdxLightStatus(int idx, uint32_t value);

private:
    int amount = 0;
    vector<Timeout *> lights;

    uint8_t GetLightsStatus();
    void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
};
#endif