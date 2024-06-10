#include "Session.h"

Session::Session(unsigned int sessionStartInMs) {
   	this->sessionStartInMs=sessionStartInMs;
}
//
Session::~Session() {
    // TODO Auto-generated destructor stub
}
//
void Session::log(String message) {
    Serial.print("log(String message:");
    Serial.println(message);
}