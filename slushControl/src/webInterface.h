#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>My first web page</TITLE>
	</HEAD>
<BODY>
	<CENTER>
			<B>Hello World.... </B>
	</CENTER>	
</BODY>
</HTML>
)=====";

WebServer server(80);

#define SSID "Slush Machinery"
#define AP_PASS "testpasswordpleasereplace"
IPAddress apIp(10, 0, 0, 1);

void initWebInterface() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIp, apIp, IPAddress(255, 255, 255, 0));
    WiFi.softAP(SSID, AP_PASS);

    server.on("/", []() {
        server.send(200, "text/html", String(MAIN_page));
    });

    server.begin();
}

void loopWebInterface() {
    server.handleClient();
}


