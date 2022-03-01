#ifndef LOGGER
#define LOGGER
/*
 * 4 levels:
 * 0 - DEBUG
 * 1 - INFO
 * 2 - WARN
 * 3 - ERROR
 * 4 - FATAL (Hopefully unused)
 */
#include "Arduino.h"
#include "SPI.h"
#include "FS.h"
#include "SD.h"
#include "uRTCLib.h"
extern const char *prefixes[5];

//     <=======  CSV is separated by semicolons  =======>     //
class Logger {
  private:
  String getTimestamp();
  uRTCLib* rtc;
  String filename;
  fs::SDFS* sd;
  public:
  Logger(String filename, uRTCLib* rtc, fs::SDFS* sd);
  void log(byte level, String message);
};
#endif
