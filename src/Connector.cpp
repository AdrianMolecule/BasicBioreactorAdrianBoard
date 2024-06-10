#include "Connector.h"
#include "Errors.h"

Connector::Connector() {
}
void Connector::initialize(JsonObject connectorJsonObject) {
	if (!connectorJsonObject.containsKey("name")) {
		Errors::raiseUserInputAlarm(String(" actuator:") + String(name) + String(" is missing the name for connector"));
	} else {
		String name = connectorJsonObject[String("name")];
		this->name = name;
	}
	String type = connectorJsonObject[String("type")];
	if (type.length() != 1) {
		String err = String("ERROR in configuration file for connector:") + String(name) + String(" the type is not one single char but length is:") + String(type.length());
		Errors::raiseUserInputAlarm(err);
	} else {
		this->type = type.charAt(0);
	}
	if (!connectorJsonObject.containsKey("pwmPin")) {
		Errors::raiseUserInputAlarm(String(" actuator:") + String(name) + String(" is missing the pwmPin"));
	} else {
		pwmPin = connectorJsonObject[("pwmPin")];
	}
	String enablePinAsString = connectorJsonObject[String("enablePin")];
	if (enablePinAsString.length() != 0) {
		enablePin = connectorJsonObject[("enablePin")];
	}
	pwmActiveOnLow = connectorJsonObject[("enableActiveOnLow")];
	enableActiveOnLow = connectorJsonObject[("enableActiveOnLow")];
}
//
void Connector::printInfo() {
	Serial.print("Connector:");
	Serial.print(name);
	Serial.print(", pwmPin:");
	Serial.print(pwmPin);
	Serial.print(", enablePin:");
	Serial.print(enablePin);
	Serial.print(", type:");
	Serial.print(type);
	Serial.print(", pwmActiveOnLow:");
	Serial.print(pwmActiveOnLow);
	Serial.print(", enableActiveOnLow:");
	Serial.print(enableActiveOnLow);
}
//
uint8_t Connector::getPwmPin() {
	return pwmPin;
}

uint8_t Connector::getEnablePin() {
	return enablePin;
}

bool Connector::isEnableActiveOnLow() {
	return enableActiveOnLow;
}

bool Connector::isPwmActiveOnLow() {
	return pwmActiveOnLow;
}

const String Connector::getName() {
	return name;
}
//
char Connector::getType() {
	return type;
}

void Connector::setType(char type) {
	this->type = type;
}
Connector::~Connector() {
}
