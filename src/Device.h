#ifndef DEVICE_H
#define DEVICE_H

using namespace std;

class Device
{
public:
    Device(uint32_t localDeviceId) : LocalDeviceID(localDeviceId)  {}

    const uint32_t LocalDeviceID; // Dispositivo local(Node ID del ESP32)
    String IpAddr = "_";
    String StationIpAddr = "_";

};

#endif // DEVICE_H