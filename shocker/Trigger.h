



//#include "Arduino.h"
#ifndef Trigger_h
#define Trigger_h


class Trigger
{
public:
  Trigger(unsigned long timeInterval_, int threshold_, bool hightolow_, int initExtreme_);  
  unsigned long timeInterval;
  int threshold;
  int extrememVal;
  
  bool hightolow;
  
  bool shouldFire(int newMeasurement, unsigned long curTime);
  
  
private:
  int maybeReplace(int newVal);
  bool doesTrigger(int val);
  unsigned long extremeRecTime;
  
  
};
#endif