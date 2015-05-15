#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

#include <Servo.h>  // servo library
//#include "Trigger.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[3] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xF0F0FFF6D2LL };

const uint64_t waterPipe = 0xF0F0FFF6D2LL;
unsigned long squirtHimCode = 1000000001;
//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;





unsigned long tripWireCode = 1122334455;
unsigned long proxCode = 1223334444;
Servo servo1;  // servo control object

const int buzzerPin = 3;
const int servoPin = 5;
int restingDegrees = 180;

unsigned long lastReset=0;
unsigned long resetInterval = 600000; //600000 = 10 min.
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

  
  
  //
  // Setup and configure rf radio
  //

///  radio.begin();

  // optionally, increase the delay between retries & # of retries
///  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  //if ( role == role_ping_out )
  {
///    radio.openWritingPipe(pipes[2]);
///    radio.openReadingPipe(1,pipes[0]);
  }
  //else
  {
    //radio.openWritingPipe(pipes[1]);
    //radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //

///  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //
///  role = role_ping_out;
 // radio.openWritingPipe(pipes[0]);
 // radio.openReadingPipe(1,pipes[1]);
///  radio.printDetails();


}

int ByteReceived;
float minutes = 0.5;
long testingTimeout = 60*1000*10;
int lightLevel=-1;
unsigned long lastReported=0;


void resetServo(){
  servo1.attach(servoPin);
  servo1.write(restingDegrees);   // Tell servo to go to 180 degrees
  delay(400);         // Pause to get it time to move

  servo1.detach();
  Serial.println("Servo Position Reset");
  
}
void maybeReset(){
  unsigned long t = millis();
  if(t > resetInterval && lastReset < (t - resetInterval)){
    resetServo();
    lastReset = t;
  }
  
}
void handleRadio(){
  if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long got_time;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got payload %lu...",got_time);
        Serial.print("got this: "); Serial.print(got_time);Serial.println();
        
        if(got_time == tripWireCode){
           if(millis() < 120000){
//              shock();
//              Serial.print(" TESTING SHOCK: Board time in minutes: ");
//              Serial.print((millis()/(1000.0*60.0)));
//              Serial.print("\nmillis(): ");
//              Serial.print(millis());
//              Serial.print("\ntimeout: ");
//              Serial.print(testingTimeout);
           }else{
//             Serial.print("Shock triggered from trip wire.\n");
//            handle_s();
           }
           //262, 294, 330, 349
           beep(1, 330);
           beep(1,262);
        }
        if(got_time == proxCode){
          beep(1, 262);
           beep(1,330);
        }
        // Delay just a little bit to let the other unit
        // make the transition to receiver
        //delay(20);
      }
  
    }
}
  
  
void loop(){
  maybeReset(); // periodically put the servo back because electrical noise tends to move it..
  handleLight();
 // handleRadio();
//Serial.print("gay");
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
    case 'r':
      handle_r();
      break;
    case 'w':
      //handle_w();
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
void beep(int count, int frq){
  int duration = 100;
  int silence = 75;
  for(int i =0; i<count; i++){
    tone(buzzerPin, frq, duration);
    delay(duration); 
    if(i == count -1){ // if this is the last beep, don't delay! This is mostly
      break;           // nice to shock immediatey after beeps.
    }
    delay(silence);
  } 
}


//SHOCK
void shock(){
  servo1.attach(servoPin);
  handleLight();
  servo1.write(55);    // Tell servo to go to 90 degrees
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

void handleLight(){return;}
void handleLightx(){
  if(millis() - lastReported > 20000){
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
void handle_r(){
  
  
}

void handle_w(){
  Serial.println("handling w");
  // First, stop listening so we can talk
  radio.stopListening();

  // Send the final one back.
  
  radio.write( &squirtHimCode, sizeof(unsigned long) );
//  printf("Sent response.\n\r");
//  Serial.println("sending shock notice for trip wire.");
  // Now, resume listening so we catch the next packets.
  radio.startListening();
}
void panic(){
  servo1.attach(servoPin);
  servo1.write(restingDegrees);   // Tell servo to go to 180 degrees
  delay(400);         // Pause to get it time to move

  servo1.detach();
  Serial.println("PANICKED!!!!!!!!!!!!!!!!!!!!!!!!!!!");
}
