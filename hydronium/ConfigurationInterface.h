#ifndef CONFIGURATION_INTERFACE
#define CONFIGURATION_INTERFACE

#include "Arduino.h"
#include "HydroniumUtil.h"
#include "Subsystems.h"
class ConfigurationInterface {
    public:
    ConfigurationInterface(InfoInterface* interface, SubsystemEntry** subsystems, uint16_t numSubsystems) : structExtractor() {
        this->interface=interface;
        this->subsystems=subsystems;
        this->numSubsystems=numSubsystems;
    }
    void process();
    void reset();
    private:
    int subsystemID=-1;
    int configID=-1;
    bool firstIter=true;
    void printOptions(int subsystemID);
    InfoInterface* interface;
    SubsystemEntry** subsystems;
    uint16_t numSubsystems;
    ReflectableStructExtractor structExtractor;
};
inline void ConfigurationInterface::printOptions(int subsystemID) {
    if(subsystemID<0) {
        this->interface->println("Configuration Interface Output: Type a number to select a subsystem.");
        for(int i=0;i<this->numSubsystems;i++) {
            this->interface->println(String(i+1)+": "+this->subsystems[i]->getName());
        }
    } else {
        this->interface->println("Configuration Interface Output: Type a number to select a value to edit.");
        SubsystemEntry* subsystem=subsystems[subsystemID];
        subsystem->reflectPersistentDataStructure(&structExtractor);
        //At this point we have the results of reflection in our ReflectableStructExtractor
        int c=0;
        for(int i=0;i<this->structExtractor.getNumResults();i++) {
            if(this->structExtractor.getResults()[i].config) {
                this->interface->print(String(c+1)+": ");
                this->interface->print(this->structExtractor.getResults()[i].name);
                this->interface->println(" ("+this->structExtractor.getResults()[i].type+")");
                c++; //Haha
            }
        }
    }
    this->interface->println("-1: <---");
}
inline void ConfigurationInterface::process() {
    if(firstIter) {
        firstIter=false;
        this->printOptions(this->subsystemID);
    }
    if(interface->available()) {
        String output=interface->readString();
        output.trim();
        int result=output.toInt();
        if(this->subsystemID==-1) {
            if((result<=0 && result!=-1) || result>numSubsystems) {
                interface->println("Unable to parse int. Please try again.");
            } else {
                interface->println("Success, recieved option "+String(result));
                this->subsystemID=(result==-1)?(-1):(result-1);
                this->printOptions(this->subsystemID);
            }
        } else {
            //We're in a subsystem
            if(configID<0) {
                if((result<=0 && result!=-1) || result>this->structExtractor.getNumResults()) {
                    interface->println("Unable to parse int. Please try again.");
                } else {
                    interface->println("Success, recieved option "+String(result));
                    if(result==-1) {
                        this->subsystemID=-1;
                        this->printOptions(this->subsystemID);
                    } else {
                        int index=0;
                        int c=0;
                        for(int i=0;i<this->structExtractor.getNumResults();i++) {
                            if(this->structExtractor.getResults()[i].config) {
                                if(c==result-1) {
                                    index=i;
                                }
                                c++; //Haha
                            }
                        }
                        configID=index;
                        ReflectedStructureField field=this->structExtractor.getResults()[configID];
                        //Get a pointer to the value
                        uint32_t ptr=subsystems[subsystemID]->getPersistentDataStoreOffset() + field.offset;
                        if(field.type.equals("ConfigurationFunctionPtr")) {
                            configID=-1;
                            //Function pointer time!
                            interface->println("Calling "+field.name);
                            ConfigurationFunctionPtr intfuncptr=*((ConfigurationFunctionPtr*)ptr);
                            subsystems[subsystemID]->getCFPM()->callFuncPtr(intfuncptr,(uint32_t)(subsystems[subsystemID]));
                        } else {
                            interface->println("Selected configurable value "+field.name);
                            interface->print("The current value is \"");
                            if(field.type.equals("int")) {
                                interface->print(String(*((int*)ptr)));
                            } else if(field.type.equals("float")) {
                                interface->print(String(*((float*)ptr)));
                            } else if(field.type.equals("long")) {
                                interface->print(String(*((long*)ptr)));
                            } else if(field.type.equals("double")) {
                                interface->print(String(*((double*)ptr)));
                            }
                            interface->println("\"");
                            interface->println("Enter a new value for the variable now. Type CANCEL to cancel.");
                        }
                    }
                }
            } else {
                if(output.equals("CANCEL")) {
                    interface->println("Operation cancelled. No values were modified.");
                } else {
                    ReflectedStructureField field=this->structExtractor.getResults()[configID];
                    //Get a pointer to the value
                    uint32_t ptr=subsystems[subsystemID]->getPersistentDataStoreOffset() + field.offset;
                    if(field.type.equals("int")) {
                        ((int*)ptr)[0]=result;
                    } else if(field.type.equals("float")) {
                        ((float*)ptr)[0]=output.toFloat();
                    } else if(field.type.equals("long")) {
                        ((long*)ptr)[0]=floor(output.toDouble());
                    } else if(field.type.equals("double")) {
                        ((double*)ptr)[0]=output.toDouble();
                    }
                    interface->println("New value set successfully.");
                }
                configID=-1;
                this->printOptions(this->subsystemID);
            }
        }
    }
}
#endif