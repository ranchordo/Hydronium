#ifndef EXAMPLEINSTRUMENT
#define EXAMPLEINSTRUMENT
#include "Subsystems.h"
#include "PersistentMemoryManager.h"

class ExampleInstrument : public Instrument<2> { //2 subsystems, that's why <2>.
  public:
  ExampleSubsystem exampleSubsystem1;
  ExampleSubsystem exampleSubsystem2;
  ExampleInstrument(): //Weird syntax, but it's a member initializer
    Instrument(&persistentMemoryBlock),
    exampleSubsystem1(2112),
    exampleSubsystem2(1221) {}
  void addAllSubsystems() override {
    this->addSubsystem(&exampleSubsystem1);
    this->addSubsystem(&exampleSubsystem2);
  }
};

#endif
