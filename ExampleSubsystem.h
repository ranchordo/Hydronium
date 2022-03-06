#ifndef EXAMPLESUBSYSTEM
#define EXAMPLESUBSYSTEM

#include "hydronium/Subsystems.h"
#include "hydronium/HydroniumUtil.h"
#include "Arduino.h"

PSTRUCT(ExampleSubsystemPersistentData,
  int somePersistentParameter=0; /*If you want to add comments to stuff you half to do it multiline like this*/
  /*Or something like this*/
  float someOtherPersistentParameter=37.48;
  CONFIG(int, configParameter)=1234; /*Use CONFIG like this so it gets flagged as a configurable value*/
  CONFIG(float, otherConfigParameter)=87.55; /*This will literally expand to "int configParameter=1234"*/
)

//If this says ExampleSubsystemPersistentDataReflectionData is not defined,
//double-check that you're using PSTRUCT to make the struct.
class ExampleSubsystem : SUBSYSTEMBASE(ExampleSubsystemPersistentData) {
  public:
  ExampleSubsystem(int someParameter) : SubsystemBase("Example subsystem") {
    this->someParameter=someParameter;
    EXTRACT_STRUCT(ExampleSubsystemPersistentData);
  }
  //When the pointer to the allocated persistent data store is handed to us
  void onPersistentPointerSet() override {
    this->persistentData->somePersistentParameter=someParameter;
  }
  private:
  int someParameter=0;
};

#endif
