#ifndef MODULE_H
#define MODULE_H

#include <Device.h>

using namespace std;

// abstract module class
class Module : public Device
{

public:
    // pure virtual method
    // virtual void speak() = 0;
    Module(const Device &device, int localModID) : Device(device), LocalModuleID(localModID)
    {
    }

    // virtual destructor
    virtual ~Module() {}

    // Numero de módulo local del dispositivo local que crea el mensaje(Monedero x, Velero X, etc)
    const int LocalModuleID;
    // Dispositivo remoto(Node ID del ESP32)
    uint32_t RemoteDeviceID;
    // ID del dispositivo remoto que crea el mensaje(Monedero x, Velero X, etc)
    int RemoteModuleID;

    virtual String UrlStatus() = 0;
    virtual String Name()=0;

    void SetRemoteDevice(uint32_t remoteDeviceId, int remoteModuleId)
    {
        RemoteDeviceID = remoteDeviceId;
        RemoteModuleID = remoteModuleId;
    }
};

#endif // MODULE_H