#ifndef COMMANDS_H_
#define COMMANDS_H_
//
#include <Arduino.h>
#include <ArduinoJson.h>
#include "SerialCommunication.h"
#include "Actuator.h"
#include "Protocol.h"
#include "Protocols.h"
#include "StorageAccess.h"

class Commands final{
private:
	static void t(Splitter *splitter);
	static void ia(Splitter *splitter);
	static void ip(Splitter *splitter);
	static void rpm(Splitter *splitter);
	static void micro(Splitter *splitter);
	static void iia(Splitter *splitter);
	static void power(Splitter *splitter);
	static void pin(Splitter *splitter);
	static void ledc(Splitter *splitter);
	static void toggleSensors(Splitter *splitter);
	static void p(Splitter *splitter);
	static void deletefile(Splitter *splitter);
	static void createFileWithContent(Splitter *splitter);
	static void showFile(Splitter *splitter);
	static void dir(Splitter *splitter);
	static void d(Splitter *splitter);
	static void iip(Splitter *splitter);
public:
	static CommandFunctionStruct commands[];
};

#endif /* COMMANDS_H_ */
#pragma once
