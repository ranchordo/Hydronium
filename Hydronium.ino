#define PRINT_DEBUG_INFO

#include "SecretConfig.h"

#include "ExampleInstrument.h"
#include "hydronium/PersistentMemoryManager.h"
#include "hydronium/Interfaces.h"
#include "uRTCLib.h"
#include "hydronium/HydroniumUtil.h"

uRTCLib rtc(0x68);
SerialInterface instrumentInterface((HardwareSerial*)&Serial, 115200, &rtc);

ExampleInstrument m(&persistentMemoryBlock, &instrumentInterface, 453456, 183274);
void setup() {
  URTCLIB_WIRE.begin();
  instrumentInterface.begin();
  m.initialize();
}

void loop() {
}