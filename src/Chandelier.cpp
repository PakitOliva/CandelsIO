#include "Chandelier.h"
#include "MeshComm.h"

Chandelier::Chandelier(const Device &localDevice, int localModuleId, int pinLatch, int pinClock, int pinData) : Module(localDevice, localModuleId), LATCH_PIN(pinLatch), CLOCK_PIN(pinClock), DATA_PIN(pinData)
{
    if (LATCH_PIN > 0 && CLOCK_PIN > 0 && DATA_PIN > 0)
    {
        pinMode(LATCH_PIN, OUTPUT);
        pinMode(CLOCK_PIN, OUTPUT);
        pinMode(DATA_PIN, OUTPUT);
    }
        // Config modules
        for (int i = 0; i < NUM_MODULES; i++)
        {
            chandelierModules.push_back(new ChandelierModule(i, DATA_PIN, CLOCK_PIN));
        }
}

void Chandelier::SetLightsOn(int num)
{
    for (int i = 0; i < num; i++)
    {
        // Busco los modulos que tienen luces a 0;
        vector<ChandelierModule *> offModules;
        for (size_t j = 0; j < chandelierModules.size(); j++)
        {
            if (chandelierModules[j]->HasAnyLightsOff())
            {
                offModules.push_back(chandelierModules[j]);
            }
        }
        if (offModules.size() == 0)
            return;

        int idxOffModule = rand() % offModules.size();
        offModules[idxOffModule]->SetOneLight();
    }
}
