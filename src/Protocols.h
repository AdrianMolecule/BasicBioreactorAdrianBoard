#ifndef PROTOCOLS_H_
#define PROTOCOLS_H_
///
#include "Arduino.h"
#include "Protocol.h"
#include "Session.h"

//const char* STANDBY_PROTOCOL_NAME="standby";

class Protocols final {
public:
	Protocols();
	virtual ~Protocols();
	static Protocol currentProtocol;
	static Session currentSession;
};

#endif /* PROTOCOLS_H_ */
#pragma once

