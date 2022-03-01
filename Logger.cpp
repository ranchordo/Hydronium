#include "Logger.h"
#include "Arduino.h"
#include "SPI.h"
#include "FS.h"
#include "SD.h"
#include "uRTCLib.h"

Logger::Logger(String filename, uRTCLib* rtc, fs::SDFS* sd) {
  this->rtc=rtc;
  this->filename=filename;
  this->sd=sd;
}
const char *prefixes[5]={"[DEBUG];", "[INFO];", "[WARN];", "[ERROR];", "[FATAL];"};
String Logger::getTimestamp() {
  return String(rtc->month())+"-"+String(rtc->day())+"-"+String(rtc->year())+";"+String(rtc->hour())+":"+String(rtc->minute())+":"+String(rtc->second())+";";
}
void Logger::log(byte level, String message) {
  File f=SD.open(this->filename,FILE_APPEND);
  String s=this->getTimestamp()+prefixes[level]+message;
  f.println(s);
  f.close();
  Serial.println(s);
}
