#ifndef EXAMPLESUBSYSTEM
#define EXAMPLESUBSYSTEM

#include "hydronium/Subsystems.h"
#include "hydronium/HydroniumUtil.h"
#include "Arduino.h"
#include "Int64String.h"

//Everything in the persistent structure needs to be created with either CONFIG or MEMBER.
//Do not put comments inside the struct.
//Use ConfigurationFunctionPtr and STATICCONFIG to create a Config Callable Function (CCF)
PSTRUCT(ExampleSubsystem,
    MEMBER(int, somePersistentParameter) = 0;
    MEMBER(float, someOtherPersistentParameter) = 37.48;
    CONFIG(int, configParameter) = 1234;
    CONFIG(float, otherConfigParameter) = 87.55;
    CONFIG(ConfigurationFunctionPtr, someConfigCallableFunction);
)
//Add configuration callable functions as shown

//If this says ExampleSubsystemPersistentDataReflectionData is not defined,
//double-check that you're using PSTRUCT to make the struct.
class SUBSYSTEMBASE(ExampleSubsystem) {
public:
    ExampleSubsystem(int someParameter): SubsystemBase("Example subsystem") {
        this->someParameter = someParameter;
    }
    void someConfigCallableFunction() { this->interface->log(0, "Hi, this is our config callable function \"someConfigCallableFunction\"!"); }
    //When the pointer to the allocated persistent data store is handed to us
    void onPersistentPointerSet() override {
        this->persistentData->somePersistentParameter = someParameter;

        //Only include this if you are using CCFs:
        INIT_CONFIG_FUNCS(ExampleSubsystem);
        //Call this for each CCF you're using:
        CREATE_CONFIG_FUNC(ExampleSubsystem, someConfigCallableFunction);
    }
private:
    int someParameter = 0;
};


#endif
