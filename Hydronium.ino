#include "ExampleInstrument.h"
#include "hydronium/PersistentMemoryManager.h"

ExampleInstrument m(&persistentMemoryBlock, 453456, 183274);
void setup() {
  //Do nothing
}

void loop() {
  m.process();
}