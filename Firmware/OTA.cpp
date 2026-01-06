#include "OTA.hpp"

void OTA::update()
{
    const char* ssid = "ROV";
    const char* password = "123456789"; // Use more than 8 characters to work!!!

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

