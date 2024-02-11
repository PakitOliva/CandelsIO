#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "MeshComm.h"

class WebApp
{
public:
    WebApp(MeshComm *meshComm) : comm(meshComm)
    {
    }
    void Init();

private:
    static String processor(const String &var);
    AsyncWebServer *webServer;
    MeshComm *comm;
};