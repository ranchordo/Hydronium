#ifndef PERSISTENT_MEMORY_MANAGER
#define PERSISTENT_MEMORY_MANAGER

#include "Arduino.h"

#define RTC_SRAM_SIZE 0x1DF0ul
union PersistentMemoryBlock {
  //Define a blank "trivial" constructor to override default nested copy one (which would throw an error)
  PersistentMemoryBlock() {};
  
  struct PersistentMemory {
    struct PersistentMemoryAssignmentInfo {
      boolean assignmentDone=false;
    } persistentMemoryAssignmentInfo;
    uint8_t persistentMemoryStart=0; //MUST BE LAST in PersistentMemory struct.
  } persistentMemory;
  char padding[RTC_SRAM_SIZE];
};
extern union PersistentMemoryBlock persistentMemoryBlock;
class PersistentMemoryManager {
  public:
  PersistentMemoryManager(PersistentMemoryBlock* memoryBlock);
  boolean needsAssignmentProcess();
  void resetAssignment();
  uint32_t addPersistentBlock(uint16_t len);
  uint32_t getNextAvailableAddress();
  void setMemoryBlock(PersistentMemoryBlock* memoryBlock);
  private:
  PersistentMemoryBlock* memoryBlock;
  uint32_t index=0;
};
#endif
