#include <ChandelierModule.h>

ChandelierModule::ChandelierModule(int id, int dataPin, int clockPin) : ID(id), DATA_PIN(dataPin), CLOCK_PIN(clockPin)
{
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        lights.push_back(new Timeout());
    }
}

uint8_t ChandelierModule::GetLightsStatus()
{
    if(testModeOn) return 0xFF;
    
    uint8_t status = 0;
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        // Usar operaciones a nivel de bits para construir el byte
        status |= (lights[i]->time_over() ? 0 : 1) << i;
    }
    return status;
}

uint32_t ChandelierModule::GetLightStatus(int idx)
{
     if(testModeOn) return 999999;

    if (idx >= NUM_LIGHTS || idx < 0)
    {
        return -1;
    }
    return lights[idx]->time_left_ms();
}
void ChandelierModule::SetIdxLightStatus(int idx, uint32_t value)
{
    if (idx >= NUM_LIGHTS || idx < 0)
    {
        return;
    }
    lights[idx]->start(value);
}

void ChandelierModule::Update()
{
    this->shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, GetLightsStatus());
    // Serial.printf("Module status ID %d ->",ID);
    // for (int i = 0; i < NUM_LIGHTS; ++i)
    //{
    //     Serial.printf(lights[i]->time_over() ? "0" : "1");
    //     Serial.printf("%d  \t",lights[i]->time_left_ms());
    // }
    // Serial.printf("\n");
}

bool ChandelierModule::HasAnyLightsOff()
{
    return GetLightsStatus() != 0xFF;
}
bool ChandelierModule::HasAllLightsOff()
{
    return GetLightsStatus() == 0x00;
}
void ChandelierModule::SetOneLight()
{
    vector<Timeout *> offLights;
    for (int i = 0; i < lights.size(); i++)
    {
        if (lights[i]->time_over())
            offLights.push_back(lights[i]);
    }
    int idxOffLights = rand() % offLights.size();
    offLights[idxOffLights]->start(900500); // 15 minutos
}

void ChandelierModule::shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (bitOrder == LSBFIRST)
            digitalWrite(dataPin, !!(val & (1 << i)));
        else
            digitalWrite(dataPin, !!(val & (1 << (7 - i))));

        digitalWrite(clockPin, HIGH);
        delayMicroseconds(20);
        digitalWrite(clockPin, LOW);
        delayMicroseconds(20);
    }
}

void ChandelierModule::ToggleTestMode(){
    testModeOn = !testModeOn;
}
