#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include "Commands.h"
#include "Protocol.h"
#include "Protocols.h"
#include "SerialCommunication.h"
#include "StorageAccess.h"
//REFERENCE IMPLEMENTATION
// see https://stackoverflow.com/questions/46111834/format-curly-braces-on-same-line-in-c-vscode for formatting
float measureTemperature();
void controlTemperature(float temp);
void durationToChars(char *, int milli);
void setupActuators();
//
char buf[800];  // for sprint use
const int TEMP_SENSOR_DELAY_BETEEN_READINGS = 5000;
// TODO wifi
// alarm when OD reaches a certain value
// todo check the config for the types
// config validation
// general modify
// todo motor direction for H add generalEnable pwmPin2  and change enablePin logic
// preferencesx
//  save config
// bug in changedeaults where they are not immediately seen in actuators

// static class member initialization from outside pretty stupid thing to need but it does not work without. Rule was removed in C++11
Protocol Protocols::currentProtocol = Protocol(false);
Session Protocols::currentSession = Session(millis());

CommandFunctionStruct Commands::commands[] = {{"t", t, "toggle-start or stop"}, {"ia", ia, "information on actuator name"}, {"ip", ip, "information on current protocol"}, {"rpm", rpm, "sets RPM for rocker"}, {"micro", micro, "sets microstepping on rocker"}, {"power", power, "sets power 0-100% for all actuators except rocker"}, {"iia", iia, "dumps all actuators info"}, {"toggleSensors", toggleSensors, "toggles display of sensor output to serial"}, {"pin", pin, " sets a pin value to 0 or 1 pin value"}, {"ledc", ledc, "executes ledc on a named actuator"}, {"p", p, "Protocol sub commands:load <name>, save -saves current, saveas <newname> -saves current protocol as new name, delete -deletes? Todo "}, {"deletefile", deletefile, "delete SPIFF file named"}, {"dir", dir, "list SPIF files in the root"}, {"createFileWithContent", createFileWithContent, "args: filename content. Creates/replace filename with new content. New content should have no NewLines\n ex: createFileWithContent \\filename.json {\"name etc"}, {"showFile", showFile, "-args filename to show like /standby"}, {"d", d, "debug"},{"iip", iip, "dumps all ?? info"},{"", NULL, "does nothing"}};
SerialCommunication serialCommunication = SerialCommunication(Commands::commands);
//
// https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers for the Eclipse Driver

// Temperature
OneWire oneWire(26);  // GPIO where the DS18B20 is connected to. Used to be GPIO36 but not anymore
DallasTemperature tempSensor(&oneWire);
/* Setting  PWM Properties */

float desiredTemperature = 37;
// some memory variables
float previousTemperature = 0.;
unsigned int startTimeMillis = esp_log_timestamp();
unsigned int temperatureHeatingStartTimeMillis = esp_log_timestamp();
float temperatureHeatingStartTemperature;
boolean heatOn = false;
boolean firstTimeTempAfterBoardStart = true;
Actuator heater = Actuator();
//
unsigned int previousMillisEsp = 0;  // in milliseconds

void setup() {
    // need to manually add the initial protocol file in SPIFF using commend createFileWithContent /prot1 {"name" where {name etc is copied from defaultProtocolStandby.json files}
    // ideally these would be on the memory card
    Serial.begin(115200);
    Serial.println("Setup Starting With ESP 1.0.6? version ==================================== ON PlatformIO--");
    Protocols::currentProtocol.load(String("standby"));  // all
    Protocols::currentProtocol.printInfo();
    return;
    // //Protocols::currentProtocol.save();
    heater = Protocols::currentProtocol.getActuatorType('H');
    heater.printInfo();
    Protocols::currentSession.sessionStartInMs = millis();
    tempSensor.setWaitForConversion(false);
    tempSensor.begin();
}

void loop() {
    serialCommunication.checkForCommand();
    return;
    unsigned int timeMillisEsp = esp_log_timestamp();
    if (timeMillisEsp > previousMillisEsp + TEMP_SENSOR_DELAY_BETEEN_READINGS) {  // read temp every 5 seconds
        float temp = measureTemperature();
        if (Protocols::currentProtocol.isControlTemperature()) {
            controlTemperature(temp);
        }
        if (!Protocols::currentProtocol.isStandby()) {
            // Do nothing except sensors
        }
        previousMillisEsp = timeMillisEsp;
    }
}
void controlTemperature(float temp) {
    if (Protocols::currentSession.startTemperature == -1000) {
        Protocols::currentSession.startTemperature = temp;
        Protocols::currentSession.log(String("starting temperature" + String(temp)));
    }
    if (temp >= Protocols::currentProtocol.getTargetTemperature()) {  // we got there
        if (heater.isStarted()) {
            heater.stop();
        }
        if (Protocols::currentSession.firstHeat) {
            Protocols::currentSession.firstHeat = false;
            Protocols::currentSession.log(String("reached target temperature" + String(Protocols::currentProtocol.getTargetTemperature())));
            Serial.printf("Reached: %f Celsius from %f after %s", temp, temperatureHeatingStartTemperature, buf);
            durationToChars(buf, esp_log_timestamp() - Protocols::currentSession.temperatureHeatingStartTimeMillis);
        }
    } else {  // need to heat
        if (!heater.isStarted()) {
            if (Protocols::currentSession.firstHeat) {
                Protocols::currentSession.temperatureHeatingStartTimeMillis = millis();
            }
            heater.start();
        }
    }
}

void durationToChars(char *buf, int milli) {
    int sec = milli / 1000;
    int hours = sec / 3600;
    int minutes = (sec - hours * 3600) / 60;
    sprintf(buf, "%d hours %d minutes %d seconds or %d total seconds\n", hours, minutes, sec - hours * 3600 - minutes * 60, sec);
}

float measureTemperature() {
    tempSensor.requestTemperaturesByIndex(0);
    float temp = tempSensor.getTempCByIndex(0);
    if (Protocols::currentSession.verboseSensors) {
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.println(" C");
    }
    if (temp > previousTemperature + 2) {
        if (Protocols::currentSession.verboseSensors) {
            Serial.print("Temperature increase from ");
            Serial.print(previousTemperature);
            Serial.print(" to ");
            Serial.println(temp);
        }
        // buzz(HIGHER);
        previousTemperature = temp;
    } else if (temp < previousTemperature - 2) {
        if (Protocols::currentSession.verboseSensors) {
            Serial.print("Temp Decrease from ");
            Serial.print(previousTemperature);
            Serial.print(" to ");
            Serial.println(temp);
        }
        // buzz(LOWER);
        previousTemperature = temp;
    }
    return temp;
}

