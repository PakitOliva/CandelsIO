#ifndef CHANDELIER_H
#define CHANDELIER_H

#include "Timeout.h"
#include "Module.h"
#include "ChandelierModule.h"

#define NUM_MODULES 8

class Chandelier : public Module
{

public:
    const int LATCH_PIN; // Pin 27 connected to ST_CP of 74HC595
    const int CLOCK_PIN; // Pin 14 connected to SH_CP of 74HC595
    const int DATA_PIN;  // Pin 13 connected to DS of 74HC595
    String Name() { return "Chandelier"; }
    static String GetName() { return "Chandelier"; }

    Chandelier(const Device &localDevice, int localModuleId, int pinLatch, int pinClock, int pinData);

    void SetLightsOn(int num);

    String UrlStatus() override
    {
        String resultado = "/chandelier.html?id=" + LocalDeviceID;
        resultado.concat("&num=" + LocalModuleID);
        return resultado;
    }

    vector<ChandelierModule *> chandelierModules;

private:
};

#endif