#include "Actuator.h"
// after changing the actuator for the OLD board to get the HBridge going
#include "Errors.h"

const int VERBOSE_PIN_VALUES = true;
/**
 * actuator Name is just a label.
 * connectorName is the label of the output port on the bioreactor v1 board, like PP1 for the first MOS or A1B1 for the first ridge output
 * start is more for debugging and specifies if the actuator is disabled or true for really starting it
 * Actuator circulation("Circulation", PP1, 40, MOS, true); //30% is about the stall power, rocker at 80% we get about 40RPM
 * Actuator air("Air", A1B1, 20, BRIDGE, true);
 */
Actuator::Actuator() {
    // Serial.println(" Actuator:: constructor");
}
Actuator::Actuator(boolean invalid) {
    this->invalid = invalid;
}
Actuator::~Actuator() {
}
// //
// Actuator::Actuator(const Actuator& other) {
//     Serial.print("Copy constructor was called on Actuator where Other name:");
//     Serial.print(other.getName());
//     Serial.print(" called from:");
//     Serial.println(__builtin_FUNCTION());
// }
//
void Actuator::initialize(JsonObject actuatorJsonObject) {
    String nam = actuatorJsonObject["name"];
    Connector connector = Connector();
    JsonObject connectorJsonObject = actuatorJsonObject["connector"];
    connector.initialize(connectorJsonObject);
    this->connector = connector;
    String n = actuatorJsonObject[String("name")];
    this->name = n;
    String type = actuatorJsonObject[String("type")];
    if (type.length() != 1) {  // to do value check in Heater, Stepper, Air, Circulation, ??
        Errors::raiseUserInputAlarm(String("ERROR in configuration file for actuator type length is:") + String(type.length()));
    } else {
        this->type = type.charAt(0);
    }
    this->power = actuatorJsonObject["power"];
    if (this->type == ROCKER_TYPE) {
        if (actuatorJsonObject.containsKey("frequency")) {
            this->frequency = actuatorJsonObject["frequency"];
        } else {
            Errors::raiseUserInputAlarm(String("An actuator of type Rocker_Type should contain a frequency which is defining the rpm"));
            return;
        }
        this->power = 50;
    } else {
        if (actuatorJsonObject.containsKey("frequency")) {
            this->frequency = actuatorJsonObject["frequency"];
        } else {
            if (connector.getType() == ConnectorType::ONE_PIN) {
                this->frequency = DEFAULT_MOS_FREQUENCY_OF_CHANGING_PWM;
            } else {                                                         // bridge
                this->frequency = DEFAULT_BRIDGE_FREQUENCY_OF_CHANGING_PWM;  // maybe here we could do better when loading /saving
            }
        }
    }
}
// UNKNOWN TYPE ERROR, type:H and beeper does not work properly with power to check on original
void Actuator::setup() {
    Serial.print(" in setup for actuator:");
    Serial.println(name);
    pinMode(connector.getPwmPin(), OUTPUT);
    if (connector.isPwmActiveOnLow()) {
        Serial.print("PwmActiveOnLow");
        Serial.println();
        dWrite(connector.getPwmPin(), HIGH);
        Serial.println("after dWrite(connector.getPwmPin(), HIGH)");
    } else {
        dWrite(connector.getPwmPin(), LOW);
    }
    if (connector.getType() == ConnectorType::TWO_PIN) {  // usually for the stepper with one PWM and one enable
        pinMode(connector.getEnablePin(), OUTPUT);
        if (connector.isEnableActiveOnLow()) {
            dWrite(connector.getEnablePin(), HIGH);
        } else {
            dWrite(connector.getEnablePin(), LOW);
        }
    }
    if (connector.getType() == ConnectorType::H_BRIDGE) {  // usually for HBridges with two PWM pins and one enable
        pinMode(connector.getEnablePin(), OUTPUT);
        if (connector.isEnableActiveOnLow()) {
            dWrite(connector.getEnablePin(), HIGH);
        } else {
            dWrite(connector.getEnablePin(), LOW);
        }
    }
    // configure PWM functionality
    ledcSetup(channel, frequency, RESOLUTION);
}

//
void Actuator::start() {
    setup();  // a hack to fix the occasional 'don't start at the first start but on the second of the rocker
    int calculatedDuty = getCalculatedDuty();
    char connectorType = connector.getType();
    switch (connectorType) {
        case ConnectorType::ONE_PIN:  // one pin
            break;
        case ConnectorType::TWO_PIN:  // Stepper_Driver with two pins
            if (connector.isEnableActiveOnLow()) {
                dWrite(connector.getEnablePin(), LOW);  // this will put pin in enable mode
            } else {
                dWrite(connector.getEnablePin(), HIGH);  // this will put pin in enable mode
            }
            break;
        case ConnectorType::H_BRIDGE:  // H bridge with two pins for 2 directions.
            if (connector.isEnableActiveOnLow()) {
                dWrite(connector.getEnablePin(), LOW);  // this will put pin in enable mode
            } else {
                dWrite(connector.getEnablePin(), HIGH);  // this will put pin in enable mode
            }
            break;
        default:
            Serial.print("UNKNOWN CONNECTOR TYPE ERROR for connector type");
            Errors::raiseUserInputAlarm(String("Error. Unknown connector TYPE:") + String(connectorType));
            return;
    }
    ledcAttachPin(connector.getPwmPin(), channel);
    switch (type) {
        case 'L':  // LED
            break;
        case BUZZER_TYPE:  // Buzz
            ledcWriteNote(channel, note_t::NOTE_E, 6);
            break;
        case ROCKER_TYPE:  // Rocker
            // dWrite(connector.getEnablePin(), LOW); // KEEP THIS as we might need it // enable disables on HIGH KEEP
            ledcSetup(channel, frequency, RESOLUTION);
            // attach the channel to the GPIO2 to be controlled
            calculatedDuty = STEPPER_DUTY_CYCLE;  // overwrite any external value
            break;
        case 'A':  // Air
        case 'C':  // Circulation
        case 'H':  // Heater
            break;
        default:
            Serial.print("UNKNOWN TYPE ERROR:");
            Serial.println(type);
            return;
    }
    ledcWrite(channel, calculatedDuty);
    this->started = true;
    printInfo();
}
//
void Actuator::stop() {
    ledcDetachPin(connector.getPwmPin());
    if (connector.isPwmActiveOnLow()) {
        Serial.println("pwmActiveOnLow");
        dWrite(connector.getPwmPin(), HIGH);
    } else {
        dWrite(connector.getPwmPin(), LOW);
    }
    switch (type) {
        case ROCKER_TYPE:  // rocker
            // dWrite(connector.getEnablePin(), HIGH); //enable disables on HIGH done in connector
            break;
    }
    char connectorType = connector.getType();
    switch (connectorType) {
        case ConnectorType::ONE_PIN:  // one pin
            ledcWrite(channel, 0);
            break;
        case ConnectorType::TWO_PIN:  // Stepper_Driver with two pins
            if (connector.isEnableActiveOnLow()) {
                dWrite(connector.getEnablePin(), HIGH);  // this will put pin in dis enable mode
            } else {
                dWrite(connector.getEnablePin(), LOW);  // this will put pin in dis enable mode
            }
            break;
        case ConnectorType::H_BRIDGE:  // H bridge with two pins for 2 directions.
            if (connector.isEnableActiveOnLow()) {
                dWrite(connector.getEnablePin(), HIGH);  //
            } else {
                dWrite(connector.getEnablePin(), LOW);  //
            }
            break;
        default:
            Serial.print("UNKNOWN CONNECTOR TYPE ERROR for connector type");
            Errors::raiseUserInputAlarm(String("Error. Unknown connector TYPE:") + String(connectorType));
            return;
    }
    this->started = false;
    this->printInfo();
}
//
void Actuator::toggle() {
    //	Serial.print(" started is:");
    //	Serial.println(this->started);
    this->printInfo();
    if (this->started == false) {
        Serial.print(name);
        Serial.println(" start");
        start();
    } else {
        Serial.print(name);
        Serial.println(" stop");
        stop();
    }
}
//
void Actuator::buzz(note_t note, int durationMs) {
    ledcWriteNote(channel, note, 6);
}
//
const String& Actuator::getName() const {
    return name;
}
//
void Actuator::setName(const String& name) {
    this->name = name;
}
//
char Actuator::getType() const {
    return type;
}
//
void Actuator::setType(char type) {
    this->type = type;
}
//
boolean Actuator::isStarted() const {
    return started;
}
//
int Actuator::getFrequency() const {
    return frequency;
}
//
void Actuator::setFrequency(int frequency) {
    this->frequency = frequency;
}
//
int8_t Actuator::getChannel() const {
    return this->channel;
}
//
void Actuator::setChannel(int8_t channel) {
    this->channel = channel;
}
//
void Actuator::dWrite(uint8_t pin, uint8_t value) {
    if (VERBOSE_PIN_VALUES) {
        Serial.print("changing the value of pin ");
        Serial.print(pin);
        Serial.print(" to:");
        Serial.println(value);
    }
    digitalWrite(pin, value);
}
//
int Actuator::getCalculatedFrequencyAsRPM() const {
    return frequency * 60 / (microstepping * STEPS_PER_ROTATION_AT_1_MICROSTEPPING);
}
const Connector& Actuator::getConnector() const {
    return connector;
}

uint8_t Actuator::getPower() const {
    return power;
}

void Actuator::setPower(uint8_t newPower) {
    this->power = newPower;
}
//
int Actuator::getCalculatedDuty() {
    int calculatedDuty = -1;
    if (connector.isPwmActiveOnLow()) {
        calculatedDuty = (100 - (int)power) * MAX_DUTY_CYCLE / 100;
    } else {
        calculatedDuty = (int)power * MAX_DUTY_CYCLE / 100;
    }
    return calculatedDuty;
}
//
int Actuator::getMicrostepping() const { return microstepping; }
//
void Actuator::setMicrostepping(int microstepping) { this->microstepping = microstepping; }
//
const void Actuator::printInfo() {
    Serial.println("Actuator dump----------------------------------------------------------------------");
    Serial.print("Actuator:");
    Serial.print(name);
    Serial.print(", actuator type:");
    Serial.print(type);
    Serial.print(", frequency:");
    Serial.print(frequency);
    if (type == ActuatorType::ROCKER_TYPE) {
        Serial.print(", (calculated frequency as RPM:");
        Serial.print(getCalculatedFrequencyAsRPM());
        Serial.print(")");
    }
    Serial.print(", power:");
    Serial.print(power);
    Serial.print(", channel:");
    Serial.print(channel);
    Serial.print(", started:");
    Serial.print(started);
    Serial.print(", invalid:");
    Serial.print(invalid);
    Serial.print(", ");
    connector.printInfo();
    Serial.print(", calculatedDuty:");
    Serial.println(getCalculatedDuty());
    Serial.print(", isStarted:");
    Serial.println(isStarted());
    Serial.println("END Actuator dump----------------------------------------------------------------------");
}
//
void Actuator::ledc() {
    int calculatedDuty = getCalculatedDuty();
    Serial.print("calculatedDuty:");
    Serial.println(calculatedDuty);
    ledcWrite(channel, calculatedDuty);
}
//
boolean Actuator::isInvalid() const {
    return invalid;
}
