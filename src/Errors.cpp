#include "Errors.h"
#include "Actuator.h"
#include "Protocols.h"

void userInputAlarmBuzz() {
    Actuator &act = Protocols::currentProtocol.getActuator("buzzer");
    if (!act.isInvalid()) {
        act.buzz(note_t::NOTE_A, 1000);
        act.start();
        delay(1000);
        act.stop();
    }
}
//
void sensorReadValueAlarmBuzz() {
    Actuator &act = Protocols::currentProtocol.getActuator("buzzer");
    if (!act.isInvalid()) {
        act.buzz(note_t::NOTE_C, 1000);
        act.start();
        delay(1000);
        act.stop();
    }
}
void developerAlarmBuzz() {
    Actuator &act = Protocols::currentProtocol.getActuator("buzzer");
    if (!act.isInvalid()) {
        act.buzz(note_t::NOTE_MAX, 1000);
        act.start();
        delay(1000);
        act.stop();
    }
}
//
void Errors::raiseUserInputAlarm(String message) {
    Serial.print("!!!!!!!!!!!! USER ERROR !!!!!!!!!!!!   ");
    Serial.println(message);
    userInputAlarmBuzz();
}
//
// void Errors::raiseDeveloperAlarm(String message) {
//	Serial.print("!!!!!!!!!!!! DEVELOPER ERROR !!!!!!!!!!!!!!!    ");
//	Serial.println(message);
//	developerAlarmBuzz();
//}

void Errors::raiseSensorReadValueAlarm(String message) {
    Serial.print("!!!!!!!!!!!! SENSOR READ VALUE ERROR !!!!!!!!!!!!    ");
    Serial.println(message);
    sensorReadValueAlarmBuzz();
}
