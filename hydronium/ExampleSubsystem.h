#ifndef EXAMPLESUBSYSTEM
#define EXAMPLESUBSYSTEM

#include "Subsystems.h"
#include "Arduino.h"

struct ExampleSubsystemPersistentData {
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
