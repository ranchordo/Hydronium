#include "Subsystems.h"

template<typename P>
void SubsystemBase<P>::setPersistentDataStore(P* persistentData) {
  this->persistentData=persistentData;
}

template<int numSubsystems>
Instrument<numSubsystems>::Instrument(PersistentMemoryBlock* memoryBlock) : memoryManager(memoryBlock) {
  
}
template<int numSubsystems>
template<typename P>
void Instrument<numSubsystems>::addSubsystem(SubsystemBase<P>* subsystem) {
  if(index>=numSubsystems) {
    //Intiailize persistent data store for this subsystem
    P* zeropointer=NULL;
    zeropointer++;
    P* persistentPointer=(P*)(this->memoryManager.addPersistentBlock((uint32_t)zeropointer));
    subsystem->setPersistentDataStore(persistentPointer);
    subsystem->onPersistentPointerSet();

    //Add to subsystems list
    this->subsystems[this->index]=subsystem;
  } else {
    //TODO: Add logger error message. We've initialized too many subsystems.
  }
  this->index++;
}
