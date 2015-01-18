#include "Trigger.h"
#include "Arduino.h"


Trigger::Trigger(unsigned long timeInterval_, int threshold_, bool hightolow_, int initExtreme_){
 timeInterval = timeInterval_;
 threshold = threshold_;
 hightolow = hightolow_;
 extrememVal = initExtreme_;
 extremeRecTime = 0;
  
  
  
}


bool Trigger::shouldFire(int newMeasurement, unsigned long curTime){

 //now check if the current light level is more than threshold brighter.
    if(doesTrigger(newMeasurement)){
      return true;
    }
    
    
    
    if(extremeRecTime < (curTime - timeInterval)){
      extrememVal = newMeasurement;
      extremeRecTime = curTime;

    }
    //at this point dimmestLight is guarenteed to be within the time interval.

    //if the curent light level is darker than the current dimmest, replace it.
    extrememVal = maybeReplace(newMeasurement); // new extremevalue if new extreme.
  
  
  
}


int Trigger::maybeReplace(int newVal){
 if(hightolow){
  return ( (extrememVal < newVal)? newVal : extrememVal); 
 }else{
  return ( (extrememVal > newVal)? newVal : extrememVal); 
 }
  
  
}

bool Trigger::doesTrigger(int val){
 if(hightolow){
   return (val < (extrememVal - threshold));
 }else{
   return (val > (extrememVal + threshold));
 }
}
  