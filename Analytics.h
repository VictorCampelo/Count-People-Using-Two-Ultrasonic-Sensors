#pragma once

class Analytics{
  public:
  Analytics();
  int _Entries;
  int _Exits;
  int _LowConfidenceEntries;
  int _LowConfidenceExits;
  int _Occupancy;

  void AddValidEntry();
  void AddValidExit();
  void AddLowConfidenceEntry();
  void AddLowConfidenceExit();
  };
