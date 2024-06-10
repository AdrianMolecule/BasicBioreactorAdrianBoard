/*
stores instruction for a run and a session used to keep temp values and a pointer to the auditTrail log
name
targetTemperature
turnLightAtOdValue
 */
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "Session.h"
#include "Actuator.h"
//
class 
Protocol final{
public:
	Protocol();
	Protocol(String name);
	Protocol(boolean invalid);
	//Protocol(const Protocol &other);
	virtual ~Protocol();
	//
	float getTurnLightAtOdValue() const;
	void setTurnLightAtOdValue(float turnLightAtOdValue = .4);
	int getTargetTemperature() const;
	void setTargetTemperature(int targetTemperature = 37);
	boolean isControlTemperature() const;
	const String& getName() const;
	void setName(const String &name);
	Session& getSession();
	boolean isStandby() const;
	boolean isInvalid() const;
	void setStandby(boolean standby);
	void load(const String path) ;
	void save(const String path) ;
	void printInfo();
	void setupActuators() const;
	//actuators
	Actuator& getActuator(String arg);
	Actuator& getActuatorType(char arg);
	const std::vector<Actuator>& getActuators();

public:
	static Session currentSession; 
private:
	String name="Uninitialized";
	int targetTemperature=-1000;//-1000 means no temperature control
	float turnLightAtOdValue=-1;
	float duration=-1;
	boolean standby=false;
	boolean invalid=false;
	std::vector<Actuator> actuators;//actuators
};

#endif /* PROTOCOL_H_ */
#pragma once
