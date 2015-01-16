
#include <Servo.h>  // servo library


Servo servo1;  // servo control object

const int buzzerPin = 10;

char names[] = { 
  'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
int frequencies[] = {
  262, 294, 330, 349, 392, 440, 494, 523};

int lightSensorPin = 0;
unsigned long lastShock = 0;
void setup()
{

  pinMode(buzzerPin, OUTPUT);
  // We'll now "attach" the servo1 object to digital pin 9.
  // If you want to control more than one servo, attach more
  // servo objects to the desired pins (must be digital).

  // Attach tells the Arduino to begin sending control signals
  // to the servo. Servos require a continuous stream of control
  // signals, even if you're not currently moving them.
  // While the servo is being controlled, it will hold its 
  // current position with some force. If you ever want to
  // release the servo (allowing it to be turned by hand),
  // you can call servo1.detach().
  Serial.begin(9600);  
  Serial.println("--- Start Serial Monitor SEND_RCVE ---");




}

int ByteReceived;
float minutes = 0.5;
long testingTimeout = 60*1000*10;
int lightLevel=-1;
unsigned long lastReported=0;

void loop(){
  handleLight();


  if (Serial.available() > 0)
  {
    ByteReceived = Serial.read();

    Serial.print(ByteReceived);   
    // Serial.print("        ");      
    //Serial.print(ByteReceived, HEX);
    Serial.print("       ");     
    Serial.print(char(ByteReceived));

    switch (char(ByteReceived)) {
    case 's':
      handle_s(); //s for shock!
      break;
    case 't':
      handle_t(); //t for test shock
      break;
    case 'b':
      handle_b(); //buzz for warning
      break;
    }



    Serial.println();    // End the line

    // END Serial Available
  }

}  
void warn(){
  beep(2); //beep 2 times.
  Serial.println("WARNING TONE");

}

void beep(int count){
  int duration = 100;
  int silence = 75;
  for(int i =0; i<count; i++){
    tone(buzzerPin, 523, duration);
    delay(duration); 
    if(i == count -1){ // if this is the last beep, don't delay! This is mostly
      break;           // nice to shock immediatey after beeps.
    }
    delay(silence);
  } 
}

int restingDegrees = 180;

//SHOCK
void shock(){
  servo1.attach(9);
  handleLight();
  servo1.write(88);    // Tell servo to go to 90 degrees
  handleLight();
  delay(500);         // Pause to get it time to move
  handleLight();
  servo1.write(restingDegrees);   // Tell servo to go to 180 degrees

    delay(1000);         // Pause to get it time to move

  servo1.detach();
  lastShock = millis();
  Serial.println("SHOCKED");

}
void handle_t(){
  if(millis() < 120000){
    shock();
    Serial.print(" TESTING SHOCK: Board time in minutes: ");
    Serial.print((millis()/(1000.0*60.0)));
    Serial.print("\nmillis(): ");
    Serial.print(millis());
    Serial.print("\ntimeout: ");
    Serial.print(testingTimeout);
  }
  else{
    Serial.println(" NO SHOCK: testing period passed.\n");
    Serial.print((millis()/(1000.0*60.0)));
    Serial.print("\nmillis(): ");
    Serial.print(millis());
    Serial.print("\ntimeout: ");
    Serial.print(testingTimeout);
  }

}
void handle_s(){
  if (millis() - lastShock > 5000){
    beep(3);
    shock();
  }
  else {
    Serial.println(" NO SHOCK: Last shock was withing 5 seconds.\n");
  }
}
void handle_b(){
  warn();
}

unsigned long timeInterval = 700;//500; //in milliseconds
int dimmestLight=0;
unsigned long dimmestLightTime=0;
int brightLight=1023;
unsigned long brightLightTime=30000000;
int deltaThreshold=300;
boolean lightOn = false;
unsigned long lightOnTime=0;
unsigned long maxPressTime = 700; // milliseconds.
int panicCounter = 0;
int maxPanics= 10;
unsigned long lightOnTimeout = 13000; //13 seconds
void handleLight(){
  if(millis() - lastReported > 10000){
    lightLevel = analogRead(lightSensorPin);
    Serial.print("LIGHT LEVEL: ");
    Serial.print(lightLevel);
    Serial.print("\n");
    Serial.print("\n");
    lastReported=millis();
  }
  int curTime=millis();
  int curLight = analogRead(lightSensorPin);

  if(!lightOn){
    
    //now check if the current light level is more than threshold brighter.
    if(curLight < (dimmestLight - deltaThreshold)){
      lightOn = true;
      brightLight = curLight;
      brightLightTime = curTime;
      Serial.print("LIGHT ON\n");
      return;
    }
    
    if(dimmestLightTime < (curTime - timeInterval)){
      dimmestLight = curLight;
      dimmestLightTime = curTime;

    }
    //at this point dimmestLight is guarenteed to be within the time interval.

    //if the curent light level is darker than the current dimmest, replace it.
    if(curLight > dimmestLight){ //it goes backwards.
      dimmestLight = curLight;
      dimmestLightTime = curTime;
    }

    
  }
  else{

    //if the current light level is darker than the current dimmest, replace it.
    if(curLight > brightLight + deltaThreshold){ //then the button is unpressed. yay!
      lightOn = false;
      panicCounter = 0; // in case any panics were tried.
      Serial.print("LIGHT OFF\n");
      dimmestLight = curLight;
      dimmestLightTime = curTime;
      return;
    }
    if(brightLightTime < (curTime - maxPressTime)){ // if we've  been pressing the light for a long time..

      if(panicCounter < maxPanics){
        panic();
        panicCounter++;
      }
      else{
        Serial.print("max panics tried. \n");
        if(curTime - brightLightTime > lightOnTimeout){//should we give up?
          Serial.print("max on time reached. Assumed light off.\n");
          lightOn = false;
          dimmestLight = curLight;
          dimmestLightTime = curTime;
          panicCounter = 0;
        }
        //Serial.print("unusual lighting!");
      }
    }

    //now check if the current light is even brighter..
    if(curLight < brightLight){
      brightLight = curLight;
      //I don't want to reset the time because we want to keep track of the the total on time.
      //but if the light gets brighter (fluctuating) I want to keep that.
    }


  }


}

void panic(){
  servo1.attach(9);
  servo1.write(restingDegrees);   // Tell servo to go to 180 degrees
  delay(400);         // Pause to get it time to move

  servo1.detach();
  Serial.println("PANICKED!!!!!!!!!!!!!!!!!!!!!!!!!!!");
}


