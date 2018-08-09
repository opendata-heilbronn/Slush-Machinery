#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "globals.h"

const char index_begin[] PROGMEM = R"=====(
<html>
	<head>
			<title>Slush Machinery</title>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width">
	</head>
    <body>
        <a href="?relaisId=0&state=0"><button>Motor Links Aus</button></a>
        <a href="?relaisId=0&state=1"><button>Motor Links An</button></a><br><br>
        <a href="?relaisId=1&state=0"><button>Kühlung Links Aus</button></a>
        <a href="?relaisId=1&state=1"><button>Kühlung Links An</button></a><br><br>
        <a href="?relaisId=2&state=0"><button>Motor Rechts Aus</button></a>
        <a href="?relaisId=2&state=1"><button>Motor Rechts An</button></a><br><br>
        <a href="?relaisId=3&state=0"><button>Kühlung Rechts Aus</button></a>
        <a href="?relaisId=3&state=1"><button>Kühlung Rechts An</button></a><br><br>
        <a href="?relaisId=4&state=0"><button>Komressor Aus</button></a>
        <a href="?relaisId=4&state=1"><button>Kompressor An</button></a><br><br>
        <a href="?relaisId=5&state=0"><button>Lüfter Aus</button></a>
        <a href="?relaisId=5&state=1"><button>Lüfter An</button></a><br><br>
        <a href="?relaisId=6&state=0"><button>Relais 7 Aus</button></a>
        <a href="?relaisId=6&state=1"><button>Relais 7 An</button></a><br><br>
        <a href="?relaisId=7&state=0"><button>Relais 8 Aus</button></a>
        <a href="?relaisId=7&state=1"><button>Relais 8 An</button></a><br><br>
        <p>TEXTTEMPERATURESENSORS</p>
        <p>TEXTRPMS</p>
    </body>
</html>
)=====";

WebServer server(80);
IPAddress apIp(10, 0, 0, 1);

SlushMachine **sms;

void parseParameters() {
    
    if(server.hasArg("relaisId") && server.hasArg("state")) {
        uint8_t relaisId = server.arg("relaisId").toInt();
        bool relaisState = server.arg("state").toInt();
        // shiftRegisterWrite(relaisId, relaisState);

        //just a test
        switch(relaisId) {
            case 0: sms[0]->setMotorState(relaisState); break; 
            case 1: sms[0]->setValveState(relaisState); break;
            case 2: sms[1]->setMotorState(relaisState); break;
            case 3: sms[1]->setValveState(relaisState); break;
            case 4: shiftRegisterWrite(BIT_COMPRESSOR, relaisState); break;
            case 5: shiftRegisterWrite(BIT_FAN, relaisState); break;
            case 6: shiftRegisterWrite(2, relaisState); break;
            case 7: shiftRegisterWrite(1, relaisState); break;
        }
    }
}

void initWebInterface(SlushMachine *slushMachineArr[]) {
    sms = slushMachineArr;
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIp, apIp, IPAddress(255, 255, 255, 0));
    WiFi.softAP(AP_SSID, AP_PASS);

    server.on("/", []() {
        String html = String(index_begin);
        html.replace("TEXTTEMPERATURESENSORS", String(sms[0]->getTemperature()) + "°C<br>" + sms[1]->getTemperature() + "°C");
        html.replace("TEXTRPMS", String(sms[0]->getMotorRevsPerMin()) + "RPM<br>" + sms[1]->getMotorRevsPerMin() + "RPM");
        parseParameters();
        server.send(200, "text/html", html);
    });

    server.begin();
}

void loopWebInterface() {
    server.handleClient();
}


