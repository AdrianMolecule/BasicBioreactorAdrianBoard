#include "Actuator.h"
#include "Protocols.h"
#include "Session.h"
#include "StorageAccess.h"
//
//
static Actuator INVALID_ACTUATOR = Actuator(true);
//const int ALL_HBRIDGE_ENABLE_PIN = 13;  // all HBridges enable pins todo move this as pin 3 in the config
//
Protocol::Protocol(boolean invalid) { this->invalid = invalid; }
Protocol::Protocol(String name) {
    Serial.print("constructor for protocol with name");
    Serial.println(name);
    this->name = name;
}
//
Protocol::Protocol() { Serial.println("in Protocol constructor"); }
//
// Protocol::Protocol(const Protocol &other) { EVIL
//     Serial.print("Copy constructor was called on Protocol where Other name:");
//     Serial.print(other.getName());
//     Serial.print(" from:");
//     Serial.println(__builtin_FUNCTION());
// }
//
Protocol::~Protocol() {
}
// reads from storage a protocol of name and makes it current
void Protocol::load(const String name) {
    this->name = name;
    String fullFileName="/"+name+".json";
    const String &content = StorageAccess::loadStringFromFile(fullFileName);
    if (content.isEmpty()) {
        Errors::raiseUserInputAlarm(String("can not find protocol:" + String(name)));
        return;
        // save the seed string to file
        // save();
        // const String &content = StorageAccess::loadStringFromFile(name);
    } else {
        Serial.println("found protocol");
        // . String read from storage:");
        // Serial.println(content);
        actuators.clear();
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, content);
        // JsonObject obj = doc.as<JsonObject>();
        String protName = doc["name"];
        this->name = protName;
        this->targetTemperature = doc["targetTemperature"];
        this->duration = doc["duration"];
        this->turnLightAtOdValue = doc["turnLightAtOdValue"];
        this->standby = doc["standby"];  // b default lack of string in json will set it to 0 or false
        JsonArray actuatorArray = doc["actuators"];
        int lastChannel = 0;
        int channel = 0;
        for (int i = 0; i < actuatorArray.size(); i++) {
            JsonObject actuatorJasonObject = actuatorArray.getElement(i);
            Actuator actuator = Actuator();
            if (lastChannel <= 15) {
                channel = lastChannel++;
            } else {
                lastChannel = -1;
                Serial.print("No More Channels");
            }
            actuator.setChannel(channel++);
            actuator.initialize(actuatorJasonObject);
            actuators.push_back(actuator);  // this calls the copy constructor todo
        }
        Protocols::currentProtocol.setupActuators();
        // printInfo();
        doc.garbageCollect();
    }
}
void Protocol::save(String newName) {
    const size_t capacity = 3000;           // JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(6) + 20; //todo replace back with capacity
    DynamicJsonDocument jsonDoc(capacity);  // todo how can I dimension this?
    Serial.print("save protocol named:");
    Serial.println(name);
    String fullFileName="/"+name+".json";
    if (name.isEmpty()) {
        jsonDoc["name"] = newName;
    } else {
        jsonDoc["name"] = name;
    }
    if (standby) {
        jsonDoc["standby"] = standby;
    }
    jsonDoc["targetTemperature"] = targetTemperature;
    jsonDoc["turnLightAtOdValue"] = turnLightAtOdValue;
    jsonDoc["duration"] = duration;
    JsonArray actuatorsJsonArray = jsonDoc.createNestedArray("actuators");
    for (const Actuator &actuator : Protocols::currentProtocol.getActuators()) {
        JsonObject actuatorJson = actuatorsJsonArray.createNestedObject();
        // JsonObject actuatorJson2 = actuatorJson.createNestedObject("actuator");
        actuatorJson["name"] = actuator.getName();
        actuatorJson["type"] = String(actuator.getType());
        if (true) {  // todo also check defaults
            actuatorJson["frequency"] = actuator.getFrequency();
        }
        if (true) {  // todo
            actuatorJson["power"] = actuator.getPower();
        }
        if (true) {  // todo
            actuatorJson["microstepping"] = actuator.getMicrostepping();
        }
        JsonObject connJson = actuatorJson.createNestedObject("connector");
        Connector conn = actuator.getConnector();
        connJson["name"] = conn.getName();
        connJson["type"] = String(conn.getType());
        if (true) {  // todo
            connJson["pwmPin"] = conn.getPwmPin();
        }
        if (true) {  // todo
            connJson["enablePin"] = conn.getEnablePin();
        }
        if (true) {  // todo
            connJson["pwmActiveOnLow"] = conn.isPwmActiveOnLow();
        }
    }
    Serial.println();
    int size = measureJsonPretty(jsonDoc);
    Serial.print("size");
    Serial.println(size);
    String s = "";
    serializeJsonPretty(jsonDoc, Serial);  // can also do in har toWrite[2000];
    serializeJsonPretty(jsonDoc, s);       // can also do in har toWrite[2000];
    jsonDoc.clear();
    StorageAccess::createFileWithContent(fullFileName,s);
    s.clear();
}

Actuator &Protocol::getActuator(String arg) {
    for (Actuator &actuator : actuators) {
        if (actuator.getName().equalsIgnoreCase(arg) == 1) {
            Serial.print("Found the actuator called:");
            Serial.println(arg);
            return actuator;
        }
    }
    // Errors::raiseUserInputAlarm(String("actuator not found:") + String(arg));
    return INVALID_ACTUATOR;
}
//
Actuator &Protocol::getActuatorType(char arg) {
    for (Actuator &actuator : actuators) {
        if (actuator.getType() == arg) {
            Serial.print("Found the actuator type called:");
            Serial.println(arg);
            return actuator;
        }
    }
    Errors::raiseUserInputAlarm(String("actuator type not found:") + String(arg));
    return INVALID_ACTUATOR;
}
//
const std::vector<Actuator> &Protocol::getActuators() { return actuators; }
//
float Protocol::getTurnLightAtOdValue() const {
    return turnLightAtOdValue;
}
//
void Protocol::setTurnLightAtOdValue(float turnLightAtOdValue) {
    this->turnLightAtOdValue = turnLightAtOdValue;
}
//
int Protocol::getTargetTemperature() const {
    return targetTemperature;
}
//
boolean Protocol::isControlTemperature() const {
    return targetTemperature == -1000;
}
// 7
void Protocol::setTargetTemperature(int targetTemperature) {
    this->targetTemperature = targetTemperature;
}
//
void Protocol::setName(const String &name) {
    this->name = name;
}
//
boolean Protocol::isStandby() const {
    return standby;
}
//
void Protocol::setStandby(boolean standby) {
    this->standby = standby;
}
//
boolean Protocol::isInvalid() const {
    return invalid;
}
//
const String &Protocol::getName() const {
    return name;
}
//
void Protocol::printInfo() {  // todo change
    Serial.println("Protocol dump----------------------------------------------------------------------");
    Serial.print("Protocol:");
    Serial.print(name);
    Serial.print(", standby:");
    Serial.print(standby);
    Serial.print(", targetTemperature:");
    Serial.print(targetTemperature);
    Serial.print(", turnLightAtOdValue:");
    Serial.print(turnLightAtOdValue);
    Serial.print(", duration:");
    Serial.println(duration);
    for (Actuator &actuator : actuators) {
        actuator.printInfo();
    }
    Serial.println("END Protocol dump----------------------------------------------------------------------");
    Serial.println();
}
//
void Protocol::setupActuators() const {
    Serial.println("Setup PWM Actuators===============");
   //pinMode(ALL_HBRIDGE_ENABLE_PIN, OUTPUT);         // PWM H_BRIDGES
    //Actuator::dWrite(ALL_HBRIDGE_ENABLE_PIN, HIGH);  // stays like this
    std::vector<Actuator> acts = Protocols::currentProtocol.getActuators();
    for (Actuator &actuator : acts) {
        actuator.setup();
    }
}
