#ifndef PERSISTENT_MEMORY_MANAGER
#define PERSISTENT_MEMORY_MANAGER

#include "Arduino.h"

#define RTC_SRAM_SIZE 0x1DF0ul
union PersistentMemoryBlock {
  //Define a blank "trivial" constructor to override default nested copy one (which would throw an error)
  PersistentMemoryBlock() {};
  
  struct PersistentMemory {
    bool needToInitializeStructures=true;
    uint8_t persistentMemoryStart=0; //MUST BE LAST in PersistentMemory struct.
  } persistentMemory;
  char padding[RTC_SRAM_SIZE];
};
extern union PersistentMemoryBlock persistentMemoryBlock;
class PersistentMemoryManager {
  public:
  PersistentMemoryManager(PersistentMemoryBlock* memoryBlock);
  void resetAssignment();
  uint32_t addPersistentBlock(uint32_t len);
  uint32_t getNextAvailableAddress();
  void setMemoryBlock(PersistentMemoryBlock* memoryBlock);
  void confirmStructuresInitialized() {this->memoryBlock->persistentMemory.needToInitializeStructures=false;}
  bool needToInitializeStructures() {return this->memoryBlock->persistentMemory.needToInitializeStructures;}
  PersistentMemoryBlock* getMemoryBlock();
  private:
  PersistentMemoryBlock* memoryBlock;
  uint32_t index=0;
};
#endif
