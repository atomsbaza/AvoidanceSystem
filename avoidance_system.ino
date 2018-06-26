#include "Servo.h"
#include <NewPing.h>

//int Front;
int f;
int r;
int l;
int b;

// Assign your channel in pins
#define CHANNEL3_IN_PIN 3 // Channel 1
#define CHANNEL4_IN_PIN 2 // Channel 2
#define CHANNEL5_IN_PIN 4 // Channel 5

// Assign your channel out pins
#define CHANNEL3_OUT_PIN 9  // Channel 1
#define CHANNEL4_OUT_PIN 10 // Channel 2
#define CHANNEL5_OUT_PIN 11 // Channel 5

// Servo objects generate the signals expected by Electronic Speed Controllers and Servos
// We will use the objects to output the signals we read in

Servo servoChannel3; // Channel 1
Servo servoChannel4; // Channel 2
Servo servoChannel5; // Channel 5

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define CHANNEL1_FLAG 1
#define CHANNEL2_FLAG 2
#define CHANNEL3_FLAG 4
#define CHANNEL4_FLAG 8
#define CHANNEL5_FLAG 16
#define CHANNEL6_FLAG 32
#define CHANNEL7_FLAG 64
#define CHANNEL8_FLAG 128

// holds the update flags defined above
volatile uint32_t bUpdateFlagsShared;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals
volatile uint32_t unChannel1InShared;
volatile uint32_t unChannel2InShared;
volatile uint32_t unChannel3InShared;
volatile uint32_t unChannel4InShared;
volatile uint32_t unChannel5InShared;
volatile uint32_t unChannel6InShared;
volatile uint32_t unChannel7InShared;
volatile uint32_t unChannel8InShared;

//Pin for ultrasonic

NewPing sonar1(23, 22, 50); // Sensor 1: trigger pin, echo pin, maximum distance in cm
NewPing sonar2(27, 26, 50); // Sensor 2: same stuff
NewPing sonar3(25, 24, 50); // Sensor 3: same stuff
NewPing sonar4(29, 28, 50); // Sensor 4: same stuff
#define pingSpeed 200 // Ping frequency (in milliseconds), fastest we should ping is about 35ms per sensor
unsigned long pingTimer1, pingTimer2, pingTimer3;

void setup()
{
  Serial.begin(57600);
  
  Serial.println("multiChannels");

  // attach servo objects, these will generate the correct
  // pulses for driving Electronic speed controllers, servos or other devices
  // designed to interface directly with RC Receivers 

  servoChannel3.attach(CHANNEL3_OUT_PIN);
  servoChannel4.attach(CHANNEL4_OUT_PIN);
  servoChannel5.attach(CHANNEL5_OUT_PIN);

  // attach the interrupts used to read the channels
  attachInterrupt(CHANNEL3_IN_PIN, calcChannel3,CHANGE);
  attachInterrupt(CHANNEL4_IN_PIN, calcChannel4,CHANGE);
  attachInterrupt(CHANNEL5_IN_PIN, calcChannel5,CHANGE);
}

void loop(){

  int Front = sonar1.ping_cm();
  int Right = sonar3.ping_cm();
  int Left  = sonar2.ping_cm();
  int Back  = sonar4.ping_cm();
  int a = 1;
  if(a == 0){  
  Serial.print("Front = ");
  Serial.println(Front);
  delay(1000);
  Serial.print("Right = ");
  Serial.println(Right);
  delay(1000);
  Serial.print("Left = ");
  Serial.println(Left);
  delay(1000);
  Serial.print("Back = ");
  Serial.println(Back);
  delay(1000);
  }
  

  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained
  // between calls to loop.
  
  static uint32_t unChannel1In;
  static uint32_t unChannel2In;
  static uint32_t unChannel3In;
  static uint32_t unChannel4In;
  static uint32_t unChannel5In;
  static uint32_t unChannel6In;
  static uint32_t unChannel7In;
  static uint32_t unChannel8In;
  
  // local copy of update flags
  static uint32_t bUpdateFlags;

  // check shared update flags to see if any channels have a new signal
  if(bUpdateFlagsShared)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    bUpdateFlags = bUpdateFlagsShared;
   
    // in the current code, the shared values are always populated
    // so we could copy them without testing the flags
    // however in the future this could change, so lets
    // only copy when the flags tell us we can.
   
    if(bUpdateFlags & CHANNEL1_FLAG)
    {
      unChannel1In = unChannel1InShared;
    }
   
    if(bUpdateFlags & CHANNEL2_FLAG)
    {      

      unChannel2In = unChannel2InShared;
    }
   
    if(bUpdateFlags & CHANNEL3_FLAG)
    {
      unChannel3In = unChannel3InShared;
    }
    
    if(bUpdateFlags & CHANNEL4_FLAG)
    {
      unChannel4In = unChannel4InShared;
    }
   
    if(bUpdateFlags & CHANNEL5_FLAG)
    {
      unChannel5In = unChannel5InShared;
    }
   
    if(bUpdateFlags & CHANNEL6_FLAG)
    {
      unChannel6In = unChannel6InShared;
    }
    
    if(bUpdateFlags & CHANNEL7_FLAG)
    {
      unChannel7In = unChannel7InShared;
    }
   
    if(bUpdateFlags & CHANNEL8_FLAG)
    {
      unChannel8In = unChannel8InShared;
    }
    
    // clear shared copy of updated flags as we have already taken the updates
    // we still have a local copy if we need to use it in bUpdateFlags
    bUpdateFlagsShared = 0;
   
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on
    // as soon as interrupts are back on, we can no longer use the shared copies, the interrupt
    // service routines own these and could update them at any time. During the update, the
    // shared copies may contain junk. Luckily we have our local copies to work with :-)
  }
  
  // do any processing from here onwards
  // only use the local values unChannel1, unChannel2, unChannel3, unChannel4, unChannel5, unChannel6, unChannel7, unChannel8
  // variables unChannel1InShared, unChannel2InShared, etc are always owned by the 
  // the interrupt routines and should not be used in loop
  
  if(bUpdateFlags & CHANNEL1_FLAG)
  {
      // remove the // from the line below to implement pass through updates to the servo on this channel -
      //servoChannel1.writeMicroseconds(unChannel1In);
  }

  if(bUpdateFlags & CHANNEL2_FLAG)
  {
      // remove the // from the line below to implement pass through updates to the servo on this channel -
      //servoChannel2.writeMicroseconds(unChannel2In);;
  }
   if(bUpdateFlags & CHANNEL3_FLAG)
   {
      if(unChannel5In <= 1490 && unChannel5In >= 1300){ // Position hold mode
        if(Front == 0 && f == 0 && Back == 0 && b == 0){
          servoChannel3.writeMicroseconds(unChannel3In);
        }
        if(Front != 0 && Back == 0 && Right == 0 && Left == 0){
          caseA1();
          Serial.println("Go back");
        }
        if(Front == 0){
          caseA2();
          Serial.println("Forward");
        }
        if(Back != 0 && Front == 0 && Right == 0 && Left == 0){
          caseA9();
          Serial.println("Forward");
        }
        if(Back == 0){
          caseA10();
          Serial.println("Go back");
        }
      }
      if(unChannel5In >= 0 && unChannel5In <= 1299){ // Manual mode, ALT mode
        int f = 0;
        int b = 0;
        if(Front == 0 && Back == 0 && Right == 0 && Left == 0){
          servoChannel3.writeMicroseconds(unChannel3In);
        }
        if(Front != 0 && Back == 0 && Right == 0 && Left == 0){
          caseA5();
          Serial.println("GO back");
        }
        if(Back != 0 && Front == 0 && Right == 0 && Left == 0){
          caseA8();
          Serial.println("Forward");
        }
        if(Left !=0 && Right !=0 && Front !=0 && Back == 0){
          caseA6();
          Serial.println("GO back");
        }
        if(Left !=0 && Right !=0 && Back !=0 && Front == 0){
          caseA7();
          Serial.println("Forward");
        }
        if(Left !=0 && Right !=0 && Back == 0 && Front == 0){
          caseA8();
          Serial.println("Forward");
        }
        if(Left !=0 && Right ==0 && Back == 0 && Front != 0){
          caseA11();
          Serial.println("Go back");
        }
        if(Left !=0 && Right ==0 && Back != 0 && Front == 0){
          caseA12();
          Serial.println("Forward");
        }
        if(Left ==0 && Right !=0 && Back == 0 && Front != 0){
          caseA11();
          Serial.println("Go back");
        }
        if(Left ==0 && Right !=0 && Back != 0 && Front == 0){
          caseA12();
          Serial.println("Forward");
        }
      }
      if(unChannel5In >= 1750){ // Loiter mode
        int f = 0;
        int b = 0;
        if(Front == 0 && Back == 0 && Right == 0 && Left == 0){
          servoChannel3.writeMicroseconds(unChannel3In);
        }
        if(Front != 0 && Back == 0 && Right == 0 && Left == 0){
          caseA5();
          Serial.println("GO back");
        }
        if(Back != 0 && Front == 0 && Right == 0 && Left == 0){
          caseA8();
          Serial.println("Forward");
        }
        if(Left !=0 && Right !=0 && Front !=0 && Back == 0){
          caseA6();
          Serial.println("GO back");
        }
        if(Left !=0 && Right !=0 && Back !=0 && Front == 0){
          caseA7();
          Serial.println("Forward");
        }
        if(Left !=0 && Right !=0 && Back == 0 && Front == 0){
          caseA8();
          Serial.println("Forward");
        }
        if(Left !=0 && Right ==0 && Back == 0 && Front != 0){
          caseA11();
          Serial.println("Go back");
        }
        if(Left !=0 && Right ==0 && Back != 0 && Front == 0){
          caseA12();
          Serial.println("Forward");
        }
        if(Left ==0 && Right !=0 && Back == 0 && Front != 0){
          caseA11();
          Serial.println("Go back");
        }
        if(Left ==0 && Right !=0 && Back != 0 && Front == 0){
          caseA12();
          Serial.println("Forward");
        }
     }
   } 
    if(bUpdateFlags & CHANNEL4_FLAG)
    {
      if(unChannel5In <= 1490 && unChannel5In >= 1300){ // Position hold mode
        if(Right == 0 && r == 0 && Left == 0 && l == 0){
          servoChannel4.writeMicroseconds(unChannel4In);
        }
        if(Right != 0 && Left == 0){
          caseB1();
          Serial.println("Go left");
        }
        if(Right == 0){
          caseB2();
          Serial.println("Go right");
        }
        if(Left != 0 && Right == 0){
          caseC1();
          Serial.println("Go right");
        }
        if(Left == 0){
          caseC2();
          Serial.println("Go left");
        }
      }
      if(unChannel5In >= 0 && unChannel5In <= 1299){ // Manual mode, ALT mode
        int r = 0;
        int l = 0;
        if(Right == 0 && Left == 0 && Back == 0 && Front == 0){
          servoChannel4.writeMicroseconds(unChannel4In);
        }
        if(Right != 0 && Left == 0 && Back == 0 && Front == 0){
          caseB3();
          Serial.println("Go left");
        }
        if(Left != 0 && Right == 0 && Back == 0 && Front == 0){
          caseC3();
          Serial.println("Go right");
        }
        if(Front != 0 && Back != 0 && Left != 0 && Right == 0){
          caseC6();
          Serial.println("Go right");
        }
        if(Front != 0 && Back != 0 && Right != 0 && Left == 0){
          caseB6();
          Serial.println("Go left");
        }
        if(Front != 0 && Back != 0 && Right == 0 && Left == 0){
          caseB3();
          Serial.println("Go left");
        }
        if(Left !=0 && Right ==0 && Back == 0 && Front != 0){
          caseB7();
          Serial.println("Go right");
        }
        if(Left !=0 && Right ==0 && Back != 0 && Front == 0){
          caseB7();
          Serial.println("Go right");
        }
        if(Left ==0 && Right !=0 && Back == 0 && Front != 0){
          caseC7();
          Serial.println("Go left");
        }
        if(Left ==0 && Right !=0 && Back != 0 && Front == 0){
          caseC7();
          Serial.println("Go left");
        }
      }
      if(unChannel5In >= 1750){ // Loiter mode
        int r = 0;
        int l = 0;
        if(Right == 0 && Left == 0 && Back == 0 && Front == 0){
          servoChannel4.writeMicroseconds(unChannel4In);
        }
        if(Right != 0 && Left == 0 && Back == 0 && Front == 0){
          caseB3();
          Serial.println("Go left");
        }
        if(Left != 0 && Right == 0 && Back == 0 && Front == 0){
          caseC3();
          Serial.println("Go right");
        }
        if(Front != 0 && Back != 0 && Left != 0 && Right == 0){
          caseC6();
          Serial.println("Go right");
        }
        if(Front != 0 && Back != 0 && Right != 0 && Left == 0){
          caseB6();
          Serial.println("Go left");
        }
        if(Front != 0 && Back != 0 && Right == 0 && Left == 0){
          caseB3();
          Serial.println("Go left");
        }
        if(Left !=0 && Right ==0 && Back == 0 && Front != 0){
          caseB7();
          Serial.println("Go right");
        }
        if(Left !=0 && Right ==0 && Back != 0 && Front == 0){
          caseB7();
          Serial.println("Go right");
        }
        if(Left ==0 && Right !=0 && Back == 0 && Front != 0){
          caseC7();
          Serial.println("Go left");
        }
        if(Left ==0 && Right !=0 && Back != 0 && Front == 0){
          caseC7();
          Serial.println("Go left");
        }
      }
    }
  if(bUpdateFlags & CHANNEL5_FLAG)
  {
     servoChannel5.writeMicroseconds(unChannel5In);
  }
  
  if(bUpdateFlags & CHANNEL6_FLAG)
  {
    // servoChannel6.writeMicroseconds(unChannel6In);
  }
  
  if(bUpdateFlags & CHANNEL7_FLAG)
  {
    // servoChannel7.writeMicroseconds(unChannel7In);
  }
  
  if(bUpdateFlags & CHANNEL8_FLAG)
  {
    // servoChannel8.writeMicroseconds(unChannel8In);
  }
  
  bUpdateFlags = 0;
}

void calcChannel3()
{
  static uint32_t ulStart;
  
  if(digitalRead(CHANNEL3_IN_PIN))
  {
    ulStart = micros();
  }
  else
  {
    unChannel3InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL3_FLAG;
  }
}

void calcChannel4()
{
  static uint32_t ulStart;
  
  if(digitalRead(CHANNEL4_IN_PIN))
  {
    ulStart = micros();
  }
  else
  {
    unChannel4InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL4_FLAG;
  }
}

void calcChannel5()
{
  static uint32_t ulStart;
  
  if(digitalRead(CHANNEL5_IN_PIN))
  {
    ulStart = micros();
  }
  else
  {
    unChannel5InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL5_FLAG;
  }
}


/// Case to avoidance object ///
void caseA1(){
        int F1 = 1350;
        f = f+1;
        servoChannel3.writeMicroseconds(F1);
        delay(100);
}
void caseA2(){
    if(f!=0){
      int Fa = 1650;
      servoChannel3.writeMicroseconds(Fa); 
      f = f-1;
      delay(100);
  }
}
void caseA5(){
          int F1 = 1350;
          servoChannel3.writeMicroseconds(F1);
          delay(100);
}
void caseA6(){
          int F1 = 1350;
          servoChannel3.writeMicroseconds(F1);
          delay(100);
}
void caseA7(){
          int F1 = 1650;
          servoChannel3.writeMicroseconds(F1);
          delay(100);
}
void caseA8(){
          int F1 = 1650;
          servoChannel3.writeMicroseconds(F1);
          delay(100);
}
void caseA9(){
        int F1 = 1650;
        b = b+1;
        servoChannel3.writeMicroseconds(F1);
        delay(100);
}
void caseA10(){
    if(b!=0){
      int Fa = 1350;
      servoChannel3.writeMicroseconds(Fa); 
      b = b-1;
      delay(100);
  }
}
void caseA11(){
          int F1 = 1350;
          servoChannel3.writeMicroseconds(F1);
          delay(100);
}
void caseA12(){
          int F1 = 1650;
          servoChannel3.writeMicroseconds(F1);
          delay(100);
}
void caseB1(){
          int R1 = 1350;
          r = r+1;
          servoChannel4.writeMicroseconds(R1);
          delay(100);
}

void caseB2(){
    if(r!=0){
      int Ra = 1650;
      servoChannel4.writeMicroseconds(Ra); 
      r = r-1;
      delay(100);
  }
}
void caseB3(){
        int R1 = 1350;
        servoChannel4.writeMicroseconds(R1);
        delay(100);
}
void caseB4(){
        int R1 = 1350;
        r = r+1;
        servoChannel4.writeMicroseconds(R1);
        delay(100);
}
void caseB5(){
        if(r!=0){
        int Ra = 1650;
        servoChannel4.writeMicroseconds(Ra); 
        r = r-1;
        delay(100);
  }
}
void caseB6(){
        int R1 = 1350;
        servoChannel4.writeMicroseconds(R1);
        delay(100);
}
void caseB7(){
        int R1 = 1650;
        servoChannel4.writeMicroseconds(R1);
        delay(100);
}
void caseC1(){
          int L1 = 1650;
          l = l+1;
          servoChannel4.writeMicroseconds(L1);
          delay(100);
}
void caseC2(){
     if(l!=0 ){
       int La = 1350;
       servoChannel4.writeMicroseconds(La);
       l = l-1;
       delay(100);
   }
}
void caseC3(){
      int L1 = 1650;
      servoChannel4.writeMicroseconds(L1);
      delay(100);
}
void caseC4(){
      int L1 = 1650;
      l = l+1;
      servoChannel4.writeMicroseconds(L1);
      delay(100);
}
void caseC5(){
     if(l!=0 ){
       int La = 1350;
       servoChannel4.writeMicroseconds(La);
       l = l-1;
       delay(100);
   }
}
void caseC6(){
      int L1 = 1650;
      servoChannel4.writeMicroseconds(L1);
      delay(100);
}
void caseC7(){
      int L1 = 1350;
      servoChannel4.writeMicroseconds(L1);
      delay(100);
}

