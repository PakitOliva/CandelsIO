#include "WebApp.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "Chandelier.h"
#include "CaptiveRequestHandler.h"

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
                    if(!request->hasParam("idDevice")){
                        //Error en la solicitud
                        Serial.printf("Error getChandelier: Not has 'idDevice' param");
                        request->send(400);
                        return;
                    }
                    AsyncWebParameter* paramId = request->getParam("idDevice");
                    //Serial.printf("getChandelier 'id' param -> %u",paramId->value().toInt());
                    //Check if GET parameter exists
                    if(!request->hasParam("numDevice")){
                        //Error en la solicitud
                        Serial.printf("Error getChandelier: Not has 'numDevice' param");
                        request->send(400);
                        return;
                    }
                    AsyncWebParameter* paramNum = request->getParam("numDevice");
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

    webServer->on("/setTestChandelierModule", HTTP_GET, [this](AsyncWebServerRequest *request)
                  { 
                    //Check if GET parameter exists
                    if(!request->hasParam("idDevice")){
                        //Error en la solicitud
                        Serial.printf("Error getChandelier: Not has 'idDevice' param");
                        request->send(400);
                        return;
                    }
                    AsyncWebParameter* paramId = request->getParam("idDevice");

                    //Check if GET parameter exists
                    if(!request->hasParam("numDevice")){
                        //Error en la solicitud
                        Serial.printf("Error getChandelier: Not has 'numDevice' param");
                        request->send(400);
                        return;
                    }
                    AsyncWebParameter* paramNumDevice = request->getParam("numDevice");

                    //Check if GET parameter exists
                    if(!request->hasParam("numModule")){
                        //Error en la solicitud
                        Serial.printf("Error getChandelier: Not has 'numModule' param");
                        request->send(400);
                        return;
                    }                    
                    AsyncWebParameter* paramNumModule = request->getParam("numModule");

                    AsyncJsonResponse * response = new AsyncJsonResponse();
                    response->addHeader("Server","ESP Async Web Server");
                        
                    //Check if self
                    if(paramId->value().toInt() == comm->mesh.getNodeId()){
                        for (std::size_t i = 0; i < comm->modules.size(); ++i)
                        {
                            if (Chandelier *chandelier = dynamic_cast<Chandelier *>(comm->modules[i]))
                            {
                                if (chandelier->LocalDeviceID == paramId->value().toInt() && chandelier-> LocalModuleID == paramNumDevice->value().toInt())
                                {
                                    for (std::size_t j = 0; j < chandelier->chandelierModules.size(); j++) {
                                        if (chandelier->chandelierModules[j]->ID == paramNumModule->value().toInt()){
                                            chandelier->chandelierModules[j]->ToggleTestMode();
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                        }

                    }else{
                        // Send command to device
                        TestChandelierModuleCommand command = TestChandelierModuleCommand(paramId->value().toInt(), paramNumDevice->value().toInt(), paramNumModule->value().toInt());
                        comm->SendCommand(command);
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

    // setUpCaptiveRedirect();
    webServer->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP

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

void WebApp::setUpCaptiveRedirect()
{
    //======================== Webserver ========================
    // WARNING IOS (and maybe macos) WILL NOT POP UP IF IT CONTAINS THE WORD "Success" https://www.esp8266.com/viewtopic.php?f=34&t=4398
    // SAFARI (IOS) IS STUPID, G-ZIPPED FILES CAN'T END IN .GZ https://github.com/homieiot/homie-esp8266/issues/476 this is fixed by the webserver serve static function.
    // SAFARI (IOS) there is a 128KB limit to the size of the HTML. The HTML can reference external resources/images that bring the total over 128KB
    // SAFARI (IOS) popup browser has some severe limitations (javascript disabled, cookies disabled)

    // Required
    webServer->on("/connecttest.txt", [](AsyncWebServerRequest *request)
                  { request->redirect("http://logout.net"); }); // windows 11 captive portal workaround
    webServer->on("/wpad.dat", [](AsyncWebServerRequest *request)
                  { request->send(404); }); // Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

    // Background responses: Probably not all are Required, but some are. Others might speed things up?
    // A Tier (commonly used by modern systems)
    webServer->on("/generate_204", [](AsyncWebServerRequest *request)
                  { request->redirect(WiFi.softAPIP().toString().c_str()); }); // android captive portal redirect
    webServer->on("/redirect", [](AsyncWebServerRequest *request)
                  { request->redirect(WiFi.softAPIP().toString().c_str()); }); // microsoft redirect
    webServer->on("/hotspot-detect.html", [](AsyncWebServerRequest *request)
                  { request->redirect(WiFi.softAPIP().toString().c_str()); }); // apple call home
    webServer->on("/canonical.html", [](AsyncWebServerRequest *request)
                  { request->redirect(WiFi.softAPIP().toString().c_str()); }); // firefox captive portal call home
    webServer->on("/success.txt", [](AsyncWebServerRequest *request)
                  { request->send(200); }); // firefox captive portal call home
    webServer->on("/ncsi.txt", [](AsyncWebServerRequest *request)
                  { request->redirect(WiFi.softAPIP().toString().c_str()); }); // windows call home

    // B Tier (uncommon)
    //  server.on("/chrome-variations/seed",[](AsyncWebServerRequest *request){request->send(200);}); //chrome captive portal call home
    //  server.on("/service/update2/json",[](AsyncWebServerRequest *request){request->send(200);}); //firefox?
    //  server.on("/chat",[](AsyncWebServerRequest *request){request->send(404);}); //No stop asking Whatsapp, there is no internet connection
    //  server.on("/startpage",[](AsyncWebServerRequest *request){request->redirect(localIPURL);});

    // return 404 to webpage icon
    webServer->on("/favicon.ico", [](AsyncWebServerRequest *request)
                  { request->send(404); }); // webpage icon

    /*// Serve Basic HTML Page
    server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", index_html);
        response->addHeader("Cache-Control", "public,max-age=31536000");  // save this file to cache for 1 year (unless you refresh)
        request->send(response);
        Serial.println("Served Basic HTML Page");
    });*/

    // the catch all
    webServer->onNotFound([](AsyncWebServerRequest *request)
                          {
		request->redirect(WiFi.softAPIP().toString().c_str());
		Serial.print("onnotfound ");
		Serial.print(request->host());	// This gives some insight into whatever was being requested on the serial monitor
		Serial.print(" ");
		Serial.print(request->url());
		Serial.print(" sent redirect to \n"); });
}