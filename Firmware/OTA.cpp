#if defined(ARDUINO_ESP32C3_DEV) // Only for ESP32-C3

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
    // Blink LED to indicate OTA mode
    for (int i = 0; i < 1000; i++)
    {
        StatusLed::getInstance().toggle();
        delay(50);
    }

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

#endif

