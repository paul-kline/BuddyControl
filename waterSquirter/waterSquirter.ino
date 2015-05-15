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
unsigned long tripWireCode = 1122334455;
unsigned long proxCode = 1223334444;
int ledPin = 8;
int defaultPin = 13;
unsigned long timer= 0;


byte enableA = 3;
byte in1 = 2;
byte in2 = 4;


void setup()
{
  pinMode(enableA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(defaultPin, OUTPUT);
  
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
 // Serial.begin(9600);  
 // Serial.println("--- Start Serial Monitor SEND_RCVE ---");

  
  
  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

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
    //radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[2]);
  }
  //else
  {
    //radio.openWritingPipe(pipes[1]);
    //radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //
  radio.openWritingPipe(pipes[0]);
  //radio.openReadingPipe(1,waterPipe);
  radio.printDetails();


}

int ByteReceived;
float minutes = 0.5;
long testingTimeout = 60*1000*10;
int lightLevel=-1;
unsigned long lastReported=0;


  
void handleRadio(){
  if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long * message;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( message, 2*sizeof(unsigned long) );
        
        // Spew it
       // printf("Got payload %lu...",got_time);
 //       Serial.print("got this: "); Serial.print(got_time);Serial.println();
        
        if(message[0] == squirtHimCode){
           digitalWrite(enableA, HIGH);
           digitalWrite(in1, HIGH);
           digitalWrite(in2, LOW);
           delay(message[1]);
           digitalWrite(in1, LOW);
           digitalWrite(enableA, LOW);
           
          // digitalWrite(ledPin, HIGH);
          // delay(1000);
          // digitalWrite(ledPin, LOW);
          // delay(1000);
        }
        
        // Delay just a little bit to let the other unit
        // make the transition to receiver
        delay(20);
      }
  
    }
}
  
boolean on = false;  
void loop(){
  if(millis() - timer > 1000){
    timer = millis();
    digitalWrite(defaultPin, ( (on)? LOW: HIGH));
    on = !on;
  }
  handleRadio();

  if (Serial.available() > 0)
  {
    ByteReceived = Serial.read();

    Serial.print(ByteReceived);   
    // Serial.print("        ");      
    //Serial.print(ByteReceived, HEX);
    Serial.print("       ");     
    Serial.print(char(ByteReceived));

  



    Serial.println();    // End the line

    // END Serial Available
  }

}  



