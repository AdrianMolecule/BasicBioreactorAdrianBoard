#include "Commands.h"

#include "Errors.h"
#include "Protocol.h"
#include "SerialCommunication.h"

void restartIfStarted(Actuator &act) {
    if (act.isStarted()) {
        act.stop();
        act.start();
    }
}
//
void Commands::t(Splitter *splitter) {
    if (splitter->getItemCount() != 1) {
        Errors::raiseUserInputAlarm(String("this command needs the name of the actuator:"));
        return;
    }
    Actuator &act = Protocols::currentProtocol.getActuator(splitter->getItemAtIndex(0));
    if (act.isInvalid()) {
        Errors::raiseUserInputAlarm(String("This specified actuator name does not exist:"));
        return;
    }
    act.toggle();
}
//
void Commands::ia(Splitter *splitter) {
    if (splitter->getItemCount() != 1) {
        Errors::raiseUserInputAlarm(String("This command needs the name of the actuator:"));
        return;
    }
    Actuator &act = Protocols::currentProtocol.getActuator(splitter->getItemAtIndex(0));
    if (act.isInvalid()) {
        Errors::raiseUserInputAlarm(String("This specified actuator name does not exist:"));
        return;
    }
    act.printInfo();
}
//
void Commands::iia(Splitter *splitter) {
    for (Actuator actuator : Protocols::currentProtocol.getActuators()) {
        actuator.printInfo();
    }
}
//
void Commands::ip(Splitter *splitter) {
    if (Protocols::currentProtocol.isInvalid()) {
        Serial.print("currentProtocol.isInvalid !!!!");
        Protocols::currentProtocol.printInfo();
        Serial.println();
        return;
    }
    Protocols::currentProtocol.printInfo();
}
//
void Commands::micro(Splitter *splitter) {
    Actuator &act = Protocols::currentProtocol.getActuatorType(ROCKER_TYPE);
    if (act.isInvalid()) {
        Errors::raiseUserInputAlarm(String("Can not find rocker"));
        return;
    }
    if (splitter->getItemCount() == 0) {
        Serial.print("microstepping:");
        Serial.println(act.getMicrostepping());
    } else {
        int newMicrostepping = splitter->getItemAtIndex(0).toInt();
        int oldFrequency = act.getFrequency();
        int stepperFrequency = (int)oldFrequency * ((double)newMicrostepping / act.getMicrostepping());
        act.setFrequency(stepperFrequency);
        Serial.print("new frequency in pulses per second:");
        Serial.println(act.getFrequency());
        act.setMicrostepping(newMicrostepping);
        Serial.print("new microstepping:");
        Serial.println(act.getMicrostepping());
        Serial.print("or new RPM:");
        Serial.println(act.getCalculatedFrequencyAsRPM());
        restartIfStarted(act);
    }
}
//
void Commands::power(Splitter *splitter) {
    if (splitter->getItemCount() != 2) {
        Errors::raiseUserInputAlarm(String("this command needs an actuator name and a value"));
        return;
    }
    String actName = splitter->getItemAtIndex(0);
    String newPowerAsString = splitter->getItemAtIndex(1);
    int newPower = newPowerAsString.toInt();
    Actuator &act = Protocols::currentProtocol.getActuator(actName);
    if (act.isInvalid()) {
        Errors::raiseUserInputAlarm(String("Can not find rocker"));
        return;
    }
    if (act.getType() == ActuatorType::ROCKER_TYPE) {
        Errors::raiseUserInputAlarm(String("Can not set the power on rocker as it's duty must stay at 50%"));
        return;
    }
    act.setPower(newPower);
    restartIfStarted(act);
}
//
void Commands::rpm(Splitter *splitter) {
    Actuator &act = Protocols::currentProtocol.getActuator("rocker");  // todo make it more generic like set rocker requency
    if (act.isInvalid()) {
        Errors::raiseUserInputAlarm(String("Can not find rocker"));
        return;
    }
    if (splitter->getItemCount() == 0) {  // just rpm display
        Serial.print("frequency:");
        Serial.println(act.getFrequency());
        Serial.print("RPM:");
        Serial.println(act.getCalculatedFrequencyAsRPM());
        Serial.print("microstepping:");
        Serial.println(act.getMicrostepping());
    } else {
        int stepperFrequency = (int)(splitter->getItemAtIndex(0).toInt() * STEPS_PER_ROTATION_AT_1_MICROSTEPPING * act.getMicrostepping() / 60);
        Serial.print("stepperFrequency:");
        Serial.println(stepperFrequency);
        act.setFrequency(stepperFrequency);
        Serial.print("new frequency in pulses per second:");
        Serial.println(act.getFrequency());
        Serial.print("or new RPM:");
        Serial.println(act.getCalculatedFrequencyAsRPM());
        Serial.print("microstepping:");
        Serial.println(act.getMicrostepping());
        restartIfStarted(act);
    }
}
//
void Commands::toggleSensors(Splitter *splitter) {
    if (Protocols::currentSession.verboseSensors) {
        Serial.print("Turning sensors log to Serial Off");
        Serial.println();
        Protocols::currentSession.verboseSensors = false;
    } else {
        Serial.print("Turning sensors log to Serial On");
        Protocols::currentSession.verboseSensors = true;
    }
}
//
void Commands::pin(Splitter *splitter) {
    if (splitter->getItemCount() == 2) {
        Actuator::dWrite((uint8_t)splitter->getItemAtIndex(0).toInt(), (uint8_t)splitter->getItemAtIndex(1).toInt());
    } else {
        Errors::raiseUserInputAlarm("Wrong number of arguments for pin that requires pin and value");
    }
}
//
void Commands::ledc(Splitter *splitter) {
    if (splitter->getItemCount() != 1) {
        Errors::raiseUserInputAlarm(String("this command needs the name of the actuator:"));
        return;
    }
    Actuator &act = Protocols::currentProtocol.getActuator(splitter->getItemAtIndex(0));
    if (act.isInvalid()) {
        Errors::raiseUserInputAlarm(String("This specified actuator name does not exist:"));
        return;
    }
    act.ledc();
}
//
void Commands::p(Splitter *splitter) {  // todo
    if (splitter->getItemCount() != 1 && splitter->getItemCount() != 2) {
        Errors::raiseUserInputAlarm(String("this command needs the name of the protocol action and maybe a second argument, depending on the command. Only " + String(splitter->getItemCount() + String(" provided"))));
        return;
    }
    if (splitter->getItemCount() == 1) {
        String action = splitter->getItemAtIndex(0);
        if (action.equalsIgnoreCase("delete")) {
            Serial.print(" use deletefile command with the /name of the protocol as argument:");
            return;
        }
        if (action.equalsIgnoreCase("save")) {
            Protocols::currentProtocol.save("");
            return;
        }
    }
    if (splitter->getItemCount() == 2) {
        String action = splitter->getItemAtIndex(0);
        if (action.equalsIgnoreCase("load")) {
            Protocols::currentProtocol.load(splitter->getItemAtIndex(1));
            if (Protocols::currentProtocol.isInvalid()) {
                Errors::raiseUserInputAlarm(String("invalid current protocol"));
            }
            Serial.print(" loaded protocol:");
            Serial.println(splitter->getItemAtIndex(1));
            return;
        }
        if (action.equalsIgnoreCase("saveas")) {
            if (Protocols::currentProtocol.isInvalid()) {
                Errors::raiseUserInputAlarm(String("invalid current protocol"));
                return;
            }
            String oldName=Protocols::currentProtocol.getName();
            Protocols::currentProtocol.setName(splitter->getItemAtIndex(1));
            Protocols::currentProtocol.save(splitter->getItemAtIndex(1));
            Serial.print(" saved current protocol:");
            Serial.print(oldName);
            Serial.print(" as:");
            Serial.print(Protocols::currentProtocol.getName());
            return;
        }
    }
    Errors::raiseUserInputAlarm(String("wrong command or arguments"));
    return;
}
//
void Commands::deletefile(Splitter *splitter) {
    if (splitter->getItemCount() != 1) {
        Errors::raiseUserInputAlarm(String("this command needs the name of the file: like= /config.json"));
        return;
    }
    String path = splitter->getItemAtIndex(0);
    Serial.print(" deleting file: ");
    Serial.println(path);
    StorageAccess::deleteFile(path);
    // StorageAccess::dir();
}
//
void Commands::dir(Splitter *splitter) {
    StorageAccess::dir();
}
//
void Commands::createFileWithContent(Splitter *splitter) {
    if (splitter->getItemCount() != 2) {
        Errors::raiseUserInputAlarm(String("this command needs the name of the file: like= /config.json and the content without New Lines"));
        return;
    }
    String path = splitter->getItemAtIndex(0);
    Serial.print(" deleting file: ");
    Serial.println(path);
    StorageAccess::deleteFile(path);
    StorageAccess::dir();
    StorageAccess::createFileWithContent(path, splitter->getItemAtIndex(1));
}
//
void Commands::showFile(Splitter *splitter) {
      if (splitter->getItemCount() != 1) {
        Errors::raiseUserInputAlarm(String("this command needs the name of the file: like= /config.json"));
        return;
    }
    const String &content = StorageAccess::loadStringFromFile(splitter->getItemAtIndex(0));
    Serial.println("File Content");
    Serial.println(content);
    Serial.println();
}
//
void Commands::d(Splitter *splitter) {
    Actuator& a=Protocols::currentProtocol.getActuator("rocker");
    //based on pwm active on HIGH
    int channel = 0;
    Connector c= ((Connector)a.getConnector());
    int pwmPin =c.getPwmPin();
    int enablePin =c.getEnablePin();
    int frequency = 233;
   //setup
    pinMode(pwmPin, OUTPUT);
    digitalWrite(pwmPin, LOW);
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, HIGH);
    ledcSetup(a.getChannel(), a.getFrequency(), RESOLUTION);  
    // then START START
    digitalWrite(enablePin, LOW);
    ledcAttachPin(pwmPin, channel);
    ledcSetup(a.getChannel(), a.getFrequency(), RESOLUTION);
    digitalWrite(enablePin, LOW);         // enable disables on HIGH
    ledcWrite(a.getChannel(), STEPPER_DUTY_CYCLE);
}
//
void Commands::iip(Splitter *splitter) {
  if (splitter->getItemCount() != 1) {
        Errors::raiseUserInputAlarm(String("this command needs the name of the actuator:"));
        return;
    }
    Actuator &act = Protocols::currentProtocol.getActuator(splitter->getItemAtIndex(0));
    if (act.isInvalid()) {
        Errors::raiseUserInputAlarm(String("This specified actuator name does not exist:"));
        return;
    }
    act.start();
}