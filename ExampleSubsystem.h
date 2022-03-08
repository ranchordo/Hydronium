#ifndef EXAMPLESUBSYSTEM
#define EXAMPLESUBSYSTEM

#include "hydronium/Subsystems.h"
#include "hydronium/HydroniumUtil.h"
#include "Arduino.h"

//Everything in the persistent structure needs to be created with either CONFIG or MEMBER.
//Do not put comments inside the struct.
PSTRUCT(ExampleSubsystem,
  MEMBER(int, somePersistentParameter)=0;
  MEMBER(float, someOtherPersistentParameter)=37.48;
  CONFIG(int, configParameter)=1234;
  CONFIG(float, otherConfigParameter)=87.55;
  CONFIGFUNC(ExampleSubsystem, someConfigCallableFunction);
)

//If this says ExampleSubsystemPersistentDataReflectionData is not defined,
//double-check that you're using PSTRUCT to make the struct.
class SUBSYSTEMBASE(ExampleSubsystem) {
  public:
  ExampleSubsystem(int someParameter) : SubsystemBase("Example subsystem") {
    this->someParameter=someParameter;
  }
  static void someConfigCallableFunction();
  //When the pointer to the allocated persistent data store is handed to us
  void onPersistentPointerSet() override {
    this->persistentData->somePersistentParameter=someParameter;
  }
  private:
  int someParameter=0;
};

#endif
