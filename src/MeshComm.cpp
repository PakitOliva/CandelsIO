#include "MeshComm.h"
#include "Device.h"
#include <Arduino.h>
#include "Wallet.h"
#include "Chandelier.h"

void MeshComm::Init(wifi_mode_t wifiMode)
{
    // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

    mesh.init(MESH_PREFIX, MESH_PASSWORD, &localScheduler, MESH_PORT, wifiMode);
    mesh.onReceive([this](uint32_t from, String &msg) -> void
                   { receivedCallback(from, msg); });
    mesh.onNewConnection([this](uint32_t from) -> void
                         { newConnectionCallback(from); });
    mesh.onChangedConnections([this]() -> void
                              { changedConnectionCallback(); });
    mesh.onNodeTimeAdjusted([this](uint32_t from) -> void
                            { nodeTimeAdjustedCallback(from); }); // typedef std::function<void(int32_t offset)> nodeTimeAdjustedCallback_t;

    taskSendMessage.set(TASK_SECOND * 1, TASK_FOREVER, [this]() -> void
                        { SendBuffer(); });
    localScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();

    taskSendInfo.set(TASK_SECOND * 30, TASK_FOREVER, [this]() -> void
                     { SendInfo(); });
    localScheduler.addTask(taskSendInfo);
    taskSendInfo.enable();
}

void MeshComm::EnqueueMessage(uint32_t remoteDevice, int remoteID, int amount)
{
    uuid.generate();
    MsgComm msg;
    msg.ID = uuid.toCharArray();
    msg.RemoteDevice = remoteDevice;
    msg.RemoteID = remoteID;
    msg.Amount = amount;
    msg.timeOut.start(10000); // 10 segundos
    buffSent.push_back(msg);
    // Forzamos un nuevo envio;
    taskSendMessage.forceNextIteration();
}

bool MeshComm::DequeueMessage(string ID)
{
    for (size_t i = 0; i < buffReceived.size(); i++)
    {
        if (buffReceived[i].ID == ID)
        {
            buffReceived.erase(buffReceived.begin() + i);
            return true;
        }
    }
    return false;
}

vector<MsgComm> MeshComm::GetQueueMessage()
{
    return buffReceived;
}
// Needed for painless library
void MeshComm::receivedCallback(uint32_t from, String &msg)
{
    // Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
    digitalWrite(25, HIGH);
    delay(50);
    digitalWrite(25, LOW);

    if (msg.indexOf("Amount") != -1)
    {
        ProcessAmountMsg(from, msg);
    }
    else if (msg.indexOf("ipAddr") != -1)
    {
        ProcessInfoMsg(from, msg);
    }
    else
    {
        Serial.print("***** No se Reconoce el mensaje: " + msg + " \n");
    }
}

void MeshComm::newConnectionCallback(uint32_t nodeId)
{
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void MeshComm::changedConnectionCallback()
{
    Serial.printf("Changed connections\n");
}

void MeshComm::nodeTimeAdjustedCallback(int32_t offset)
{
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void MeshComm::SendBuffer()
{
    // Configuramos la frecuencia para ahorrar espacio
    if (buffSent.size() == 0)
    {
        taskSendMessage.setInterval(TASK_SECOND * 5); // 1
        // EnqueueMessage(853259969, 1, 10);
        //   Serial.printf("My mesh ID: %u \n", mesh.getNodeId());
        return;
    }
    else
    {
        taskSendMessage.setInterval(TASK_SECOND * 1);
    }

    vector<MsgComm> buffRemove;
    for (size_t i = 0; i < buffSent.size(); i++)
    {
        JsonDocument jsonBuffer;
        jsonBuffer["ID"] = buffSent[i].ID;
        jsonBuffer["RemoteDevice"] = buffSent[i].RemoteDevice;
        jsonBuffer["RemoteID"] = buffSent[i].RemoteID;
        jsonBuffer["Amount"] = buffSent[i].Amount;

        String str;
        serializeJson(jsonBuffer, str);
        if (mesh.sendSingle(buffSent[i].RemoteDevice, str))
        {
            buffRemove.push_back(buffSent[i]);
        }
        else
        {
            digitalWrite(32, HIGH);
            delay(10);
            digitalWrite(32, LOW);
        }

        // log to serial
        serializeJson(jsonBuffer, Serial);
    }

    // Eliminamos los enviados o los que tienen TimeOut
    for (size_t i = 0; i < buffRemove.size(); i++)
    {
        for (size_t j = 0; j < buffSent.size(); j++)
        {
            if (buffRemove[i].ID == buffSent[j].ID || buffSent[j].timeOut.time_over())
            {
                buffSent.erase(buffSent.begin() + j);
                break;
            }
        }
    }

    for (size_t i = 0; i < buffSent.size(); i++)
    {
        if (buffSent[i].timeOut.time_over())
        {
            buffSent.erase(buffSent.begin() + i);
            break;
        }
    }
}

void MeshComm::SendInfo()
{
    JsonDocument jsonInfo;
    jsonInfo["localDeviceID"] = mesh.getNodeId();
    jsonInfo["ipAddr"] = mesh.getAPIP().toString();
    jsonInfo["stationIpAddr"] = mesh.getStationIP().toString();

    JsonArray modulesJson = jsonInfo["modules"].to<JsonArray>();

    for (size_t i = 0; i < modules.size(); i++)
    {
        // Check if is local device
        if (modules[i]->LocalDeviceID == mesh.getNodeId())
        {
            // Si el módulo es local, lo añadimos al JSON
            JsonObject moduleJson = modulesJson.add<JsonObject>();
            moduleJson["localModuleID"] = modules[i]->LocalModuleID;
            moduleJson["remoteDeviceID"] = modules[i]->RemoteDeviceID;
            moduleJson["remoteModuleID"] = modules[i]->RemoteModuleID;
            moduleJson["typeName"] = modules[i]->Name();
            if (Chandelier *chandelier = dynamic_cast<Chandelier *>(modules[i]))
            {
                // Creamos una matriz de números
                JsonArray chandeliersModJson = moduleJson["chandelier"].to<JsonArray>();
                for (int j = 0; j < chandelier->chandelierModules.size(); j++)
                {
                    JsonArray chandelierModJson = chandeliersModJson.add<JsonArray>();
                    for (std::size_t k = 0; k < chandelier->chandelierModules[j]->NUM_LIGHTS; k++)
                    {
                        // Puedes asignar valores específicos a cada elemento de la matriz aquí
                        chandelierModJson.add(chandelier->chandelierModules[j]->GetLightStatus(k));
                    }
                }
            }
        }
    }

    String jsonStr;
    serializeJson(jsonInfo, jsonStr);
    if (!mesh.sendBroadcast(jsonStr))
    {
        Serial.print("Error mandando el mensaje de Info: " + jsonStr + "\n");
    }

    for (size_t i = 0; i < modules.size(); i++)
    {
        if (!mesh.isConnected(modules[i]->LocalDeviceID) && modules[i]->LocalDeviceID != mesh.getNodeId())
        {
            modules.erase(modules.begin() + i);
        }
    }
}

void MeshComm::ProcessInfoMsg(uint32_t from, String &msg)
{
    // Interpretamos el JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
        return;
    }

    uint32_t deviceID = doc["localDeviceID"].as<uint32_t>();
    String ipAddr = doc["ipAddr"].as<String>();
    String stationIpAddr = doc["stationIpAddr"].as<String>();
    JsonArray modulesJson = doc["modules"].as<JsonArray>();

    for (int i = 0; i < modulesJson.size(); i++)
    {
        int moduleID = modulesJson[i]["localModuleID"].as<int>();
        uint32_t remDeviceID = modulesJson[i]["remoteDeviceID"].as<int>();
        int remModuleID = modulesJson[i]["remoteModuleID"].as<int>();
        String typeName = modulesJson[i]["typeName"].as<String>();

        Module *moduleProccess = nullptr;
        for (size_t j = 0; j < modules.size(); j++)
        {
            if (modules[j]->LocalDeviceID == deviceID && modules[j]->LocalModuleID == moduleID)
            {
                moduleProccess = modules[j];
                break;
            }
        }

        if (moduleProccess == nullptr)
        {
            if (typeName == Wallet::GetName())
            {
                moduleProccess = new Wallet(Device(deviceID), moduleID, 0);
            }
            else if (typeName == Chandelier::GetName())
            {
                moduleProccess = new Chandelier(Device(deviceID), moduleID, 0, 0, 0);
            }
            else
            {
                continue;
            }
            modules.push_back(moduleProccess);
        }
        // Update values
        moduleProccess->IpAddr = ipAddr;
        moduleProccess->StationIpAddr = stationIpAddr;
        moduleProccess->SetRemoteDevice(remDeviceID, remModuleID);

        if (Chandelier *chandelier = dynamic_cast<Chandelier *>(moduleProccess))
        {
            JsonArray chandeliersModuleJson = modulesJson[i]["chandelier"].as<JsonArray>();
            for (std::size_t j = 0; j < min(chandeliersModuleJson.size(),chandelier->chandelierModules.size()); j++)
            {
                JsonArray chandelierModuleJson = chandeliersModuleJson[j].as<JsonArray>();
                for (std::size_t k = 0; k < min(chandelierModuleJson.size(), chandelier->chandelierModules[j]->NUM_LIGHTS); k++)
                {
                    // Puedes asignar valores específicos a cada elemento de la matriz aquí
                    chandelier->chandelierModules[j]->SetIdxLightStatus(k,chandelierModuleJson[k].as<uint32_t>());
                }
            }
        }
    }
}

void MeshComm::ProcessAmountMsg(uint32_t from, String &msg)
{
    // Interpretamos el JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
        return;
    }

    MsgComm msgComm;
    msgComm.ID = doc["ID"].as<string>();
    msgComm.RemoteDevice = doc["RemoteDevice"].as<uint32_t>();
    // Check if MSG is for my
    if (mesh.getNodeId() != msgComm.RemoteDevice)
    {
        return;
    }
    msgComm.RemoteID = doc["RemoteID"];
    msgComm.Amount = doc["Amount"].as<int>();
    Serial.printf("msgComm.Amount  %d \n", msgComm.ID);

    buffReceived.push_back(msgComm);
    /*for (size_t i = 0; i < buffReceived.size(); i++)
    {
        // Check if ID_MSG exist
        if (buffReceived[i].ID != msgComm.ID)
        {
            Serial.printf("ADD BuffReceived\n");
            buffReceived.push_back(msgComm);
        }
    }*/
}