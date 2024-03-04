#include "ESPAsyncWebServer.h"

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title>");
    response->printf("<meta http-equiv=\"refresh\" content=\"0;http://%s\">",WiFi.softAPIP().toString().c_str());
    //response->printf("<script>window.onload = function() {var nuevaPestana = window.open('http://%s', '_blank');};</script>",WiFi.softAPIP().toString().c_str());
    response->print("</head><body></body></html>");
    //response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    //response->print("<p>This is out captive portal front page.</p>");
    //response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    //response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    //response->print("</body></html>");
    request->send(response);
  }
};
