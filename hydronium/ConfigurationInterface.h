#ifndef CONFIGURATION_INTERFACE
#define CONFIGURATION_INTERFACE

#include "Arduino.h"
#include "HydroniumUtil.h"
#include "Subsystems.h"
class ConfigurationInterface {
    public:
    ConfigurationInterface(InfoInterface* interface, SubsystemEntry** subsystems, uint16_t numSubsystems) {
        this->interface=interface;
        this->subsystems=subsystems;
        this->numSubsystems=numSubsystems;
    }
    void process();
    void reset();
    private:
    bool firstIter=true;
    void printOptions(int subsystemID);
    InfoInterface* interface;
    SubsystemEntry** subsystems;
    uint16_t numSubsystems;
};
inline void ConfigurationInterface::printOptions(int subsystemID) {
    if(subsystemID<0) {
        this->interface->println("Configuration Interface Output: Type a number to select a subsystem.");
        for(int i=0;i<this->numSubsystems;i++) {
            this->interface->println(String(i)+": "+this->subsystems[i]->getName());
        }
    }
}
inline void ConfigurationInterface::process() {
    if(firstIter) {
        firstIter=false;
        this->printOptions(-1);
    }
}
#endif