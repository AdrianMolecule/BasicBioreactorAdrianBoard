#include <ArduinoJson.h>
#include <Arduino.h>
#ifndef CONNECTOR_H_
#define CONNECTOR_H_
enum ConnectorType {
	ONE_PIN = 'O', TWO_PIN='T', H_BRIDGE='H'
};
class Connector {
public:
	Connector();
	virtual ~Connector();
	void initialize(JsonObject);
	uint8_t getPwmPin();
	uint8_t getEnablePin();
	bool isPwmActiveOnLow();
	bool isEnableActiveOnLow();
	const String getName();
	void printInfo();
	char getType();
	void setType(char type);

private:
	String name;
	char type;
	uint8_t pwmPin = -1;
	uint8_t enablePin = -1;
	bool pwmActiveOnLow=false;
	bool enableActiveOnLow=false;
};

#endif /* CONNECTOR_H_ */
