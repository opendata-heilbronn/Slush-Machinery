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
        html.replace("REPLACE_TEMPERATURES", String(sms[0]->getTemperature()) + "°C<br>" + sms[1]->getTemperature() + "°C");
        html.replace("REPLACE_RPMS", String(sms[0]->getMotorRevsPerMin()) + "RPM<br>" + sms[1]->getMotorRevsPerMin() + "RPM");
        html.replace("REPLACE_TEMP", String(sms[0]->getSetTemperature()));
        html.replace("REPLACE_PWMFREQ", String(sms[0]->temperatureControlInterval));
        html.replace("REPLACE_PWMDUTY", String(sms[0]->maxCoolingDuty));

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
        float temp = server.arg("temp").toFloat();
        sms[0]->setTemperature(temp);
        sms[1]->setTemperature(temp);
        uint32_t pwm = server.arg("pwmFreq").toInt();
        sms[0]->temperatureControlInterval = pwm;
        sms[1]->temperatureControlInterval = pwm;
        uint32_t pwmDuty = server.arg("pwmDuty").toInt();
        sms[0]->maxCoolingDuty = pwmDuty;
        sms[1]->maxCoolingDuty = pwmDuty;

        // redirect back to home page
        server.sendHeader("Location", String("/"), true);
        server.send(302, "text/plain", "");
    });

    server.begin();
}

void loopWebInterface() {
    server.handleClient();
}


