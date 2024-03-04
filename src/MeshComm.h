#ifndef MESHCOMM_H
#define MESHCOMM_H

#include "painlessMesh.h"
#include <vector>
#include "UUID.h"
#include <Ticker.h>
#include <Timeout.h>
#include "Module.h"
#include "Commands/TestChandelierModuleCommand.h"

// Mesh
#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

using namespace std;

struct MsgComm
{
    string ID;             // UUID del paquete
    uint32_t RemoteDevice; // Dispositivo remoto(Node ID del ESP32)
    int RemoteID;          // ID del dispositivo remoto que crea el mensaje(Monedero x)
    int Amount;            // Valor que envío
    Timeout timeOut;
};

class MeshComm
{

public:
    MeshComm(vector<Module *> &mod) : modules(mod)
    {
    }

    void EnqueueMessage(uint32_t remoteDevice, int remoteID, int amount);
    bool DequeueMessage(string ID);
    vector<MsgComm> GetQueueMessage();

    void Init(wifi_mode_t wifiMode = WIFI_MODE_APSTA);
    painlessMesh mesh;
    void receivedCallback(uint32_t from, String &msg);
    void newConnectionCallback(uint32_t nodeId);
    void changedConnectionCallback();
    void nodeTimeAdjustedCallback(int32_t offset);
    vector<Module *> &modules;
    void SendCommand(MyCommand &command);
    void ProcessCommandMsg(uint32_t from, String &msg);

private:
    vector<MsgComm> buffSent;
    vector<MsgComm> buffReceived;
    UUID uuid;
    Scheduler localScheduler; // Control task
    Task taskSendMessage;
    Task taskSendInfo;

    void SendBuffer();
    void SendInfo();
    void ProcessInfoMsg(uint32_t from, String &msg);
    void ProcessAmountMsg(uint32_t from, String &msg);
};

#endif // MESHCOMM_H
