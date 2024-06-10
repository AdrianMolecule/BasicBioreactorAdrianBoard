#ifndef ACTUATOR_H_
#define ACTUATOR_H_
//
#include <Arduino.h>
#include "Connector.h"

const int RESOLUTION = 10; //Resolution 8 does not seem to work, 10, 12, 15
// L298N (1A,1B) output connector is 18, 16 (2A,2B) is 4,15, (3A, 3B) is 33,23, 	(4A, 4B) :5,14
const int MAX_DUTY_CYCLE = (int) (pow(2, RESOLUTION) - 1);
// duty cycle for stepper is always 50%
const int STEPPER_DUTY_CYCLE = MAX_DUTY_CYCLE / 2;
const int DEFAULT_MOS_FREQUENCY_OF_CHANGING_PWM = 4000;
const int DEFAULT_BRIDGE_FREQUENCY_OF_CHANGING_PWM = 1000;
const int STEPS_PER_ROTATION_AT_1_MICROSTEPPING = 200;

enum ActuatorType {
	BUZZER_TYPE = 'B', CIRCULATION_TYPE = 'C', HEATER_TYPE = 'H', ROCKER_TYPE = 'R'
};
//
class Actuator final {
public:
	Actuator();
	Actuator(boolean invalid);
	//Actuator(const Actuator &other); evil
	virtual ~Actuator();
	static void dWrite(uint8_t pin, uint8_t value);
	void setLastChannel();
	void initialize(JsonObject);
	const void printInfo();
	void setup();
	void start();
	void toggle();
	void stop();
	void setPower(uint8_t newPower);
	void buzz(note_t note, int durationMs);
	void ledc();
	const String& getName() const;
	void setName(const String &name);
	char getType() const;
	void setType(char type = '*');
	boolean isStarted() const;
	void setFrequency(int frequency);
	int getFrequency() const;
	int getCalculatedFrequencyAsRPM() const;
	int getCalculatedDuty();
	const Connector& getConnector() const;
	uint8_t getPower() const;
	int getMicrostepping() const;
	void setMicrostepping(int microstepping);
	void setChannel(	int8_t channel );
	int8_t getChannel() const;
	boolean isInvalid() const;
private:
	String name="Uninitialized";
	char type = '-';
	Connector connector;
	uint8_t power = 50; //needed for rocker and fits into a less than 255 uint8
	int frequency=0;
	int microstepping=1;
	int8_t channel = -1;
	boolean started = false;
	boolean invalid=false;
};

#endif /* ACTUATOR_H_ */
#pragma once
