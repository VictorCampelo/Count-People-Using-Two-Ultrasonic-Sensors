#include "Analytics.h"

Analytics::Analytics(){
  _Entries = 0;
  _Exits = 0;
  _LowConfidenceEntries = 0;
  _LowConfidenceExits = 0;
  _Occupancy = 0;
  };

void Analytics::AddValidEntry(){
  _Entries++;
  _Occupancy++;
  };
void Analytics::AddLowConfidenceEntry(){
  _LowConfidenceEntries++;
  _Occupancy++;
  };
void Analytics::AddValidExit(){
  _Exits++;
  _Occupancy--;
  };
void Analytics::AddLowConfidenceExit(){
  _LowConfidenceExits++;
  _Occupancy--;
  };
