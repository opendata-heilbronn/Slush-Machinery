#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "globals.h"

// yes, this works. C++11 magic
const char index_begin[] PROGMEM = 
#include "website.html"
;

WebServer server(80);
IPAddress apIp(10, 0, 0, 1);

SlushMachine **sms;

void initWebInterface(SlushMachine *slushMachineArr[]) {
    sms = slushMachineArr;
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIp, apIp, IPAddress(255, 255, 255, 0));
    WiFi.softAP(AP_SSID, AP_PASS);

    server.on("/", []() {
        String html = String(index_begin);
        html.replace("REPLACE_CURTEMP_0", String(sms[0]->getTemperature()) + "°C");
        html.replace("REPLACE_CURTEMP_1", String(sms[1]->getTemperature()) + "°C");
        html.replace("REPLACE_RPM_0", String(sms[0]->getMotorRevsPerMin()) + "RPM");
        html.replace("REPLACE_RPM_1", String(sms[1]->getMotorRevsPerMin()) + "RPM");
        html.replace("REPLACE_SETTEMP_0", String(sms[0]->getSetTemperature()));
        html.replace("REPLACE_SETTEMP_1", String(sms[1]->getSetTemperature()));
        html.replace("REPLACE_PWMDUTY_0", String(sms[0]->maxCoolingDuty));
        html.replace("REPLACE_PWMDUTY_1", String(sms[1]->maxCoolingDuty));
        html.replace("REPLACE_PWMFREQ", String(sms[0]->temperatureControlInterval));

        server.send(200, "text/html", html);
    });

    server.on("/setOutput", []() {
        if (server.hasArg("relaisId") && server.hasArg("state")) {
            uint8_t relaisId = server.arg("relaisId").toInt();
            bool relaisState = server.arg("state").toInt();
            // shiftRegisterWrite(relaisId, relaisState);

            switch(relaisId) {
                case 0: sms[0]->setMotorState(relaisState); break; 
                case 1: sms[0]->setCooling(relaisState); break;
                case 2: sms[1]->setMotorState(relaisState); break;
                case 3: sms[1]->setCooling(relaisState); break;
                case 4: shiftRegisterWrite(BIT_COMPRESSOR, relaisState); break;
                case 5: shiftRegisterWrite(BIT_FAN, relaisState); break;
                case 6: shiftRegisterWrite(2, relaisState); break;
                case 7: shiftRegisterWrite(1, relaisState); break;
            }
        }

        // redirect back to home page
        server.sendHeader("Location", String("/"), true);
        server.send(302, "text/plain", "");
    });

    server.on("/configValues", []() {
        // parse parameters naively and hope that it works
        sms[0]->setTemperature(server.arg("temp0").toFloat());
        sms[1]->setTemperature(server.arg("temp1").toFloat());
        sms[0]->maxCoolingDuty = server.arg("pwmDuty0").toInt();
        sms[1]->maxCoolingDuty = server.arg("pwmDuty1").toInt();
        uint32_t pwm = server.arg("pwmFreq").toInt();
        sms[0]->temperatureControlInterval = pwm;
        sms[1]->temperatureControlInterval = pwm;

        // redirect back to home page
        server.sendHeader("Location", String("/"), true);
        server.send(302, "text/plain", "");
    });

    server.begin();
}

void loopWebInterface() {
    server.handleClient();
}


