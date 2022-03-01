#include "PersistentMemoryManager.h"
RTC_DATA_ATTR union PersistentMemoryBlock persistentMemoryBlock;

PersistentMemoryManager::PersistentMemoryManager(PersistentMemoryBlock* memoryBlock) {
  this->setMemoryBlock(memoryBlock);
  this->resetAssignment();
}
void PersistentMemoryManager::setMemoryBlock(PersistentMemoryBlock* memoryBlock) {
  this->memoryBlock=memoryBlock;
}
boolean PersistentMemoryManager::needsAssignmentProcess() {
  return !(this->memoryBlock->persistentMemory.persistentMemoryAssignmentInfo.assignmentDone);
}
void PersistentMemoryManager::resetAssignment() {
  this->index=(uint32_t)(&this->memoryBlock->persistentMemory.persistentMemoryStart);
}
uint32_t PersistentMemoryManager::getNextAvailableAddress() {
  return this->index;
}
uint32_t PersistentMemoryManager::addPersistentBlock(uint16_t len) {
  this->index+=len;
  return this->index-len;
}
