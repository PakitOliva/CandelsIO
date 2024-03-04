#include <Arduino.h>
#include <Wire.h>
#include "Wallet.h"
#include "Chandelier.h"
#include "MeshComm.h"
#include "WebApp.h"
#include <typeinfo>
#include <DNSServer.h>


// Status LED
const int LED_RED_PIN = 32;
const int LED_GREEN_PIN = 33;
const int LED_BLUE_PIN = 25;

vector<Module *> modules;
MeshComm comm(modules);
WebApp webApp(&comm);
DNSServer dnsServer;

Ticker periodicTicker;
void processModules();
void readConfigModules();

void setup()
{
  Serial.begin(115200);

  comm.Init();

  readConfigModules();

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  periodicTicker.attach_ms(500, processModules);
  webApp.Init();

  dnsServer.start(53, "*", WiFi.softAPIP());
}

void loop()
{
  // it will run the user scheduler as well
  comm.mesh.update();
  dnsServer.processNextRequest();

}

void processModules()
{
  for (std::size_t i = 0; i != modules.size(); ++i)
  {
    if (modules[i]->LocalDeviceID != comm.mesh.getNodeId())
      continue;

    modules[i]->IpAddr = comm.mesh.getAPIP().toString();
    modules[i]->StationIpAddr = comm.mesh.getStationIP().toString();

    if (Wallet *wallet = dynamic_cast<Wallet *>(modules[i]))
    {
      if (wallet->isReady())
      {
        int value = wallet->GetAndClean();
        // comm.EnqueueMessage(853259969, 1, value);
        comm.EnqueueMessage(wallet->RemoteDeviceID, wallet->RemoteModuleID, value);
      }
    }
    else if (Chandelier *chandelier = dynamic_cast<Chandelier *>(modules[i]))
    {
      digitalWrite(chandelier->LATCH_PIN, LOW); // ground latchPin and hold low for as long as you are transmitting
      for (int i = 0; i < chandelier->chandelierModules.size(); i++)
      {
        chandelier->chandelierModules[i]->Update();
      }
      // return the latch pin high to signal chip that it
      // no longer needs to listen for information
      delayMicroseconds(20);
      digitalWrite(chandelier->LATCH_PIN, HIGH); // pull the latchPin to save the data
      delayMicroseconds(20);
      digitalWrite(chandelier->LATCH_PIN, LOW); // pull the latchPin to save the data

      vector<MsgComm> listMsgs = comm.GetQueueMessage();
      for (int i = 0; i < listMsgs.size(); i++)
      {
        chandelier->SetLightsOn(listMsgs[i].Amount);
        comm.DequeueMessage(listMsgs[i].ID);
      }

      // Comprobamos si todos estan apagados y encendemos 5 al azar
      bool allOff = true;
      for (size_t i = 0; i < chandelier->chandelierModules.size(); i++)
      {
        allOff = allOff && chandelier->chandelierModules[i]->HasAllLightsOff();
      }
      if (allOff)
      {
        chandelier->SetLightsOn(5);
      }
    }
    else
    {
      Serial.printf("PROCESS MODULE UNKOWN\n");
    }
  }
}

void readConfigModules()
{
  Device deviceInstance = Device(comm.mesh.getNodeId());
  /*
  Wallet *wallet1 = new Wallet(deviceInstance, 1, 27);
  wallet1->SetRemoteDevice(853259969, 1);
  /**/
  /**/
  Chandelier *wallet1 = new Chandelier(deviceInstance, 1, 27, 14, 13);
  wallet1->SetRemoteDevice(853258853, 1);
  /**/
  modules.push_back(wallet1);
}