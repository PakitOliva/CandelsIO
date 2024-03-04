#ifndef MYCOMMAND_H
#define MYCOMMAND_H

using namespace std;

// abstract module class
class MyCommand
{

public:
    // pure virtual method
    MyCommand(uint32_t deviceId, int moduleId) : DeviceID(deviceId), ModuleID(moduleId)
    {
    }
    virtual ~MyCommand() {}

    const uint32_t DeviceID; // Dispositivo local(Node ID del ESP32)
    // Numero de módulo local del dispositivo local que crea el mensaje(Monedero x, Velero X, etc)
    const int ModuleID;
};

#endif // MYCOMMAND_H