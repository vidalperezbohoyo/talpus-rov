#include "OTA.hpp"

void OTA::init()
{
    pinMode(PIN_OTA, INPUT_PULLUP);
}

bool OTA::requestUpdate()
{
    return digitalRead(PIN_OTA) == LOW;
}

void OTA::update()
{
    const char* ssid = OTA_SSID;
    const char* password = OTA_PASSWORD;

    WebServer server(80);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();

    server.on("/", [&server]() {
        server.send(200, "text/plain", "Load firmware on 192.168.4.1/update");
    });

    ElegantOTA.begin(&server);
    server.begin();

    while (true) 
    {
        server.handleClient();
        ElegantOTA.loop();
    }
}

