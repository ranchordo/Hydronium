#ifndef HYDRONIUM_UTIL
#define HYDRONIUM_UTIL

#include "Arduino.h"
#include "uRTCLib.h"
#include "esp_wifi.h"
#include "WiFi.h"
#include "Interfaces.h"
#include "time.h"


#ifndef ENV_WIFI_SSID
#define ENV_WIFI_SSID "No SSID provided."
#endif

#ifndef ENV_WIFI_PWD
#define ENV_WIFI_PWD ""
#endif

class HydroniumUtil {
    public:
    static char *toCStr(String s);
    static bool setRTCWithNTP(InfoInterface* interface, uRTCLib *rtc);
};

inline char *HydroniumUtil::toCStr(String s) {
    return (char *)s.c_str();
}

inline bool HydroniumUtil::setRTCWithNTP(InfoInterface* interface, uRTCLib *rtc) {
    esp_wifi_start();
    WiFi.mode(WIFI_STA);
    WiFi.begin(HydroniumUtil::toCStr(ENV_WIFI_SSID), HydroniumUtil::toCStr(ENV_WIFI_PWD));
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
    interface->println("Connecting to " + String(ENV_WIFI_SSID));
    #endif
    delay(500);
    byte count = 0;
    while (WiFi.status() != WL_CONNECTED && count < 20)
    {
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
        interface->print(".");
    #endif
        delay(500);
        count++;
    }
    if (count >= 20)
    {
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
        interface->println("Failed to connect to wifi network.");
    #endif
        WiFi.disconnect(true);
        WiFi.mode(WIFI_MODE_NULL);
        esp_wifi_deinit();
        esp_wifi_stop();
        return false;
    }
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
    interface->println("");
    #endif
    configTime(-25200, 0, "pool.ntp.org");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
        interface->println("Failed to get local time from ntp.");
    #endif
        WiFi.disconnect(true);
        WiFi.mode(WIFI_MODE_NULL);
        esp_wifi_stop();
        esp_wifi_deinit(); // So hardware entropy sources will work later
        return false;
    }
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
    interface->print(String(timeinfo.tm_hour & 0xFF));
    interface->print(":");
    interface->print(String(timeinfo.tm_min & 0xFF));
    interface->print(":");
    interface->print(String(timeinfo.tm_sec & 0xFF));
    interface->print(", ");
    interface->print(String(timeinfo.tm_hour & 0xFF));
    interface->print("/");
    interface->print(String(timeinfo.tm_mon & 0xFF));
    interface->print("/");
    interface->print(String(timeinfo.tm_mday & 0xFF));
    interface->print("/");
    interface->println(String((timeinfo.tm_year % 100) & 0xFF));
    #endif
    rtc->set(
        timeinfo.tm_sec & 0xFF,
        timeinfo.tm_min & 0xFF,
        timeinfo.tm_hour & 0xFF,
        timeinfo.tm_wday & 0xFF,
        timeinfo.tm_mday & 0xFF,
        timeinfo.tm_mon & 0xFF,
        (timeinfo.tm_year % 100) & 0xFF); // Actually set the RTC
    WiFi.disconnect(true);
    WiFi.mode(WIFI_MODE_NULL);
    esp_wifi_stop();
    esp_wifi_deinit(); // So hardware entropy sources will work later
    return true;
}


#include "Regexp.h"

#define CONFIG_REGEXP "CONFIG%(%s*([^,]+)%s*,%s*([^,]+)%s*%)"
#define MAX_INFO_LEN 32
#define MAXCAPTURES 4

#define MAX_CONFIG_PARAMETERS 64

#define EXTRACT_STRUCT(name) ReflectableStructExtractor::extractStructInformation( name ## ReflectionData );

struct ConfigurationField {
    String type;
    String name;
};

class ReflectableStructExtractor {
    public:
    static void structExtractionRegexMatchCallback(const char* match, const unsigned int length, const MatchState& ms);
    static char regexpCaptureBuffer[MAX_INFO_LEN];
    static void extractStructInformation(const String reflectionData);
    static ConfigurationField* getResults() {return results;}
    static uint8_t getNumResults() {return numResults;}
    private:
    static MatchState ms;
    static ConfigurationField results[MAX_CONFIG_PARAMETERS];
    static uint8_t numResults;
};
MatchState ReflectableStructExtractor::ms;
uint8_t ReflectableStructExtractor::numResults{0};
char ReflectableStructExtractor::regexpCaptureBuffer[MAX_INFO_LEN];
ConfigurationField ReflectableStructExtractor::results[MAX_CONFIG_PARAMETERS];

inline void ReflectableStructExtractor::extractStructInformation(const String reflectionData) {
    numResults=0;
    ms.Target(HydroniumUtil::toCStr(reflectionData));
    ms.GlobalMatch(CONFIG_REGEXP, &ReflectableStructExtractor::structExtractionRegexMatchCallback);
}
inline void ReflectableStructExtractor::structExtractionRegexMatchCallback(const char * match, const unsigned int length, const MatchState& ms) {
    ms.GetCapture(regexpCaptureBuffer,0);
    results[numResults].type=String(regexpCaptureBuffer);
    ms.GetCapture(regexpCaptureBuffer,1);
    results[numResults].name=String(regexpCaptureBuffer);
    numResults++;
}

#endif