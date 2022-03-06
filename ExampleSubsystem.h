#ifndef EXAMPLESUBSYSTEM
#define EXAMPLESUBSYSTEM

#include "hydronium/Subsystems.h"
#include "Arduino.h"

struct ExampleSubsystemConfiguration {
  int test;
};

struct ExampleSubsystemPersistentData : public SubsystemPersistentData<ExampleSubsystemConfiguration> {
  int somePersistentParameter=0;
};
class ExampleSubsystem : public SubsystemBase<ExampleSubsystemPersistentData> {
  public:
  ExampleSubsystem(int someParameter) {
    this->someParameter=someParameter;
  }
  //When the pointer to the allocated persistent data store is handed to us
  void onPersistentPointerSet() override {
    this->persistentData->somePersistentParameter=someParameter;
  }
  private:
  int someParameter=0;
};

#endif
