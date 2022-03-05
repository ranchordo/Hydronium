#ifndef EXAMPLEINSTRUMENT
#define EXAMPLEINSTRUMENT
#include "hydronium/Instrument.h"
#include "ExampleSubsystem.h"
#include "hydronium/PersistentMemoryManager.h"

class ExampleInstrument : public Instrument<2> { //2 subsystems, that's why <2>.
  public:
  ExampleSubsystem exampleSubsystem1;
  ExampleSubsystem exampleSubsystem2;
  ExampleInstrument(PersistentMemoryBlock* memoryBlock, int param1, int param2): //Weird syntax, but it's a member initializer
    Instrument(memoryBlock),
    exampleSubsystem1(param1),
    exampleSubsystem2(param2) {}
  void addAllSubsystems() override {
    this->addSubsystem(&exampleSubsystem1);
    this->addSubsystem(&exampleSubsystem2);
  }
};

#endif
