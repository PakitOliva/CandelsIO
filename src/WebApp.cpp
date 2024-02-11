#include "WebApp.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "Chandelier.h"

void WebApp::Init()
{
    webServer = new AsyncWebServer(80);

    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred whiles mounting SPIFFS");
        return;
    }

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());

    // Route for root / web page
    webServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/index.html", String(), false, processor); });
    // Route to load style.css file
    webServer->on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/styles.css", "text/css"); });
    webServer->on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/scripts.js"); });

    webServer->on("/getModules", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { 
                    AsyncJsonResponse * response = new AsyncJsonResponse();
                    response->addHeader("Server","ESP Async Web Server");
                    JsonObject deviceJson = response->getRoot(); 
                    deviceJson["deviceId"] = comm->mesh.getNodeId();
                    JsonArray modulesJson = deviceJson["modules"].to<JsonArray>();
                    for(size_t i = 0; i< comm->modules.size(); i++){
                        JsonObject moduleJson = modulesJson.add<JsonObject>();
                        moduleJson["name"] = comm->modules[i]->Name();
                        moduleJson["url"] = comm->modules[i]->UrlStatus();
                        moduleJson["ipAddr"] = comm->modules[i]->IpAddr;
                        moduleJson["stationIpAddr"] = comm->modules[i]->StationIpAddr;
                    }

                    serializeJson(deviceJson, Serial);
                    response->setLength();
                    request->send(response); });

    // Html chandelier
    webServer->on("/chandelier.html", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/chandelier.html", String(), false, processor); });
    // Route to load chandelier.css file
    webServer->on("/chandelier.css", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/chandelier.css", "text/css"); });
    webServer->on("/chandelier.js", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/chandelier.js"); });
    webServer->on("/getChandelier", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { 
                    //Check if GET parameter exists
                    if(!request->hasParam("id")){
                        //Error en la solicitud
                        Serial.printf("Error getChandelier: Not has 'id' param");
                        request->send(400);
                        return;
                    }
                    AsyncWebParameter* paramId = request->getParam("id");
                    //Serial.printf("getChandelier 'id' param -> %u",paramId->value().toInt());
                    //Check if GET parameter exists
                    if(!request->hasParam("num")){
                        //Error en la solicitud
                        Serial.printf("Error getChandelier: Not has 'num' param");
                        request->send(400);
                        return;
                    }
                    AsyncWebParameter* paramNum = request->getParam("num");
                    //Serial.printf("getChandelier 'num' param -> %u",paramNum->value().toInt());

                    AsyncJsonResponse * response = new AsyncJsonResponse();
                    response->addHeader("Server","ESP Async Web Server");
                    JsonObject objJson = response->getRoot(); 
                    JsonArray modulesJson = objJson["modules"].to<JsonArray>();

                    for (std::size_t i = 0; i < comm->modules.size(); ++i)
                    {
                        if (Chandelier *chandelier = dynamic_cast<Chandelier *>(comm->modules[i]))
                        {
                            if (chandelier->LocalDeviceID == paramId->value().toInt() && chandelier-> LocalModuleID == paramNum->value().toInt())
                            {
                                for (std::size_t j = 0; j < chandelier->chandelierModules.size(); j++) {
                                    JsonArray row = modulesJson.add<JsonArray>();
                                    for (std::size_t k = 0; k < chandelier->chandelierModules[j]->NUM_LIGHTS; k++) {
                                        // Puedes asignar valores específicos a cada elemento de la matriz aquí
                                        row.add(chandelier->chandelierModules[j]->GetLightStatus(k)/1000);      
                                    }
                                }
                            }
                        }
                    }
                    //serializeJson(objJson, Serial);
                    response->setLength();
                    request->send(response); });

    // Html chandelier
    webServer->on("/wallet.html", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/wallet.html", String(), false, processor); });
    // Route to load chandelier.css file
    webServer->on("/wallet.css", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/wallet.css", "text/css"); });
    webServer->on("/wallet.js", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/wallet.js"); });
    webServer->on("/getWallet", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { 
                    AsyncJsonResponse * response = new AsyncJsonResponse();
                    response->addHeader("Server","ESP Async Web Server");
                    JsonObject modulesJson = response->getRoot(); 

                    JsonArray modules = modulesJson["modules"].to<JsonArray>();
                    for (int i = 0; i < 4; i++) {
                        JsonArray row = modules.add<JsonArray>();
                        for (int j = 0; j < 8; j++) {
                            // Puedes asignar valores específicos a cada elemento de la matriz aquí
                            row.add((int)random(10)); // Generar un número aleatorio entre 0 y 10
                        }
                    }

                    serializeJson(modulesJson, Serial);
                    response->setLength();
                    request->send(response); });

    // Start server
    webServer->begin();
}

// Replaces placeholder with LED state value
String WebApp::processor(const String &var)
{
    if (var == F("MODULE_NAME"))
    {
        return "Test1";
    }

    if (var == F("NAME_MODULE"))
    {
        return "Test2";
    }

    if (var == "MY_ID")
    {
        return "";
    }
    return String();
}
