#include "EventLog.h"
#include "Analytics.h"

enum CurrentState{
  Idle=0,
  EntryA = 1,
  EntryB = 2,
  EntryC = 3,
  ExitA = 4,
  ExitB = 5,
  ExitC = 6,
  PossibleError = 7,
  Error = 8,
  PossibleEntry = 9,
  PossibleExit = 10,
  LowConfidenceEntry = 11,
  LowConfidenceExit = 12,
  ValidEntry = 13,
  ValidExit = 14,
  SpuriousReadout = 15,
  UncertainReadout = 16
  };

enum CurrentState StateMatrix[][4] = {
{Idle, EntryA, ExitA, PossibleError}, //Idle
{Idle, EntryA, EntryC, EntryB}, //EntryA
{LowConfidenceEntry, EntryA, EntryC, EntryB}, //EntryB
{ValidEntry, EntryA, EntryC, EntryB}, //EntryC
{Idle, ExitC, ExitA, ExitB}, //ExitA
{LowConfidenceExit, ExitC, ExitA, ExitB}, //ExitB
{ValidExit, ExitC, ExitA, ExitB}, //ExitC
{SpuriousReadout, EntryA, ExitA, Error}, //PossibleError
{UncertainReadout, Error, Error, Error},   //Error
{Idle,Idle,Idle,Idle}, //PossibleEntry
{Idle,Idle,Idle,Idle}, //PossibleExit
{Idle,Idle,Idle,Idle}, //LowConfidenceEntry
{Idle,Idle,Idle,Idle}, //LowConfidenceExit
{Idle,Idle,Idle,Idle}, //ValidEntry
{Idle,Idle,Idle,Idle}, //ValidExit
{Idle,Idle,Idle,Idle}, //SpuriousReadout
{Idle,Idle,Idle,Idle} //UncertainReadout
};




class StateMachine{
  public:
    StateMachine(int MinRange = 0, int MaxRange = 100);
    enum CurrentState _Context;
    int _MinRange;
    int _MaxRange;
    
    void Update(int InsideReadout, int OutsideReadout, EventLog* logs, Analytics* analytics);
    uint8_t CheckRange(int distance);
    
  private:
     enum CurrentState _State;
};
