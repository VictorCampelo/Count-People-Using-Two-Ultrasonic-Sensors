//how many logs are stored in the controller's memory.
//each entry should consume sizeof(struct LogEntry) bytes in RAM (currently 5? check -fshort-enums compiler flag)
#define LOG_ENTRY_COUNT 500 //keep in mind we don't have a lot of RAM available.
//Although we could use SPIFFS to potentially have up to a few MB available, i can't really be bothered to figure out how to at the moment. -Ricardo
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#pragma once

struct Eventslist{
enum Type{
  Blank = 0,
  Initialization = 1,
  ValidEntry = 2,
  ValidExit = 3,
  LowConfEntry = 4,
  LowConfExit = 5,
  SpuriousReadout = 6,
  UncertainReadout = 7,
  EchoFailure = 8, //still at odds on including this one.
  PIRDetection = 9
  };
};
typedef Eventslist::Type Event;

struct LogEntry{
  uint32_t timestamp;
  Event type;
  };

class EventLog{
  public:
    EventLog();
    struct LogEntry _Entries[LOG_ENTRY_COUNT];
    uint16_t _NextEntry; // doesn't need to be larger than 2 bytes due to lack of RAM
    void AddEntry(struct LogEntry entry);
    void SendLogs(ESP8266WebServer* server); //sends a page containing binary log data
  };
