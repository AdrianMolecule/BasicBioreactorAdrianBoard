#include "Protocol.h"

#ifndef A_H_
#define A_H_

class Session final {
public:
	Session(unsigned int );
	virtual ~Session();
		void log(String message);

	public:
		float startTemperature;
		boolean verboseSensors=true;
		boolean firstHeat=true;// todo this is more like it was not yet heated to the target temperature
		unsigned int temperatureHeatingStartTimeMillis;
		unsigned int sessionStartInMs;
		//String logevent todo
};

#endif /* A_H_ */
