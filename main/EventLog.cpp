#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "EventLog.h"

EventLog::EventLog(){
  _NextEntry = 0;
  memset(_Entries, 0, sizeof(struct LogEntry));
  AddEntry({millis(), Event::Initialization}); 
  
  
  };

void EventLog::AddEntry(struct LogEntry entry){
  _Entries[_NextEntry] = entry;
  _NextEntry++;
  if (_NextEntry >= LOG_ENTRY_COUNT){
    _NextEntry = 0;
    };
  };

void EventLog::SendLogs(ESP8266WebServer* server){
  uint32_t timestamp = millis();
  server->send(200, "application/octet-stream");
  server->sendContent_P((const char*)&timestamp, (size_t)sizeof(uint32_t));
  //server.sendContent((const char*)&_Entries[_NextEntry], (size_t)sizeof(struct LogEntry)*(LOG_ENTRY_COUNT-_NextEntry)); //TODO check possible off-by-one error?
  //server.sendContent((const char*)&_Entries[0], (size_t)sizeof(struct LogEntry)*(NextEntry+1));
  server->sendContent_P((const char*)_Entries, (size_t)(sizeof(LogEntry)*LOG_ENTRY_COUNT));
  //TODO check whether i have to (and how to?) use setContentLength
  }
