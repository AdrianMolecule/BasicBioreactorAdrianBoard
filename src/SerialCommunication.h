#ifndef SERIALCOMMUNICATION_H_
#define SERIALCOMMUNICATION_H_

#include <Arduino.h>
#include "Splitter.h"
//
struct CommandFunctionStruct {
	const char *commandName;
	void (*func)(Splitter *extras);
	const char *commandHelp;
};

class SerialCommunication final {
public:
	SerialCommunication(CommandFunctionStruct*);
	virtual ~SerialCommunication();
	void checkForCommand();
	void executeCommand(char *token, String extra);
private:
	CommandFunctionStruct *commands;
};

#endif /* SERIALCOMMUNICATION_H_ */
