//This is the first time i've tried to write actual C++.

//Beware.

#include "StateMachine.h"
#include "EventLog.h"
#include "Analytics.h"
#include <Arduino.h>
StateMachine::StateMachine(int MinRange, int MaxRange){
  _MinRange = MinRange;
  _MaxRange = MaxRange;
  _Context = Idle;
  };


uint8_t StateMachine::CheckRange(int distance){
  if ((distance >= _MinRange)&&(distance <= _MaxRange)){
    return 1;
    } else {return 0;};
  };

void StateMachine::Update(int InsideReadout, int OutsideReadout, EventLog* logs, Analytics* analytics){
  uint8_t data = (CheckRange(InsideReadout)<<1) | (CheckRange(OutsideReadout));
  
  //a bunch of specific actions to take within a particular state
  switch(_Context){
    case Idle: break;
    case EntryA: break;
    case EntryB: break;
    case EntryC:break;
    case ExitA: break;
    case ExitB: break;
    case ExitC: break;
    case PossibleError: break;
    case Error:
    break;
    case PossibleEntry: break;
    case PossibleExit: break;
    case LowConfidenceEntry: 
      logs->AddEntry({millis(), Event::LowConfEntry});
      analytics->AddLowConfidenceEntry();
    break;
    case LowConfidenceExit:
      logs->AddEntry({millis(), Event::LowConfExit});
      analytics->AddLowConfidenceExit();
    break;
    case ValidEntry:
      logs->AddEntry({millis(), Event::ValidEntry});
      analytics->AddValidEntry();
    break;
    case ValidExit:
      logs->AddEntry({millis(), Event::ValidExit});
      analytics->AddValidExit();
    break;
    case SpuriousReadout:
      logs->AddEntry({millis(), Event::SpuriousReadout});
    break;
    case UncertainReadout:
      logs->AddEntry({millis(), Event::UncertainReadout});
    break;
    default:
    Serial.println("Error: Attempt to process an unknown state!");
    break;
    };
  //updates machine context
  _Context = StateMatrix[_Context][data];
  
  }
