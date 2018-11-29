/*******************************************************************************
This is GID Gizmo project by Group 22

*******************************************************************************/

#include <MPR121.h>
#include <Wire.h>
//#include "Compiler_Errors.h"
#include <Servo.h>
// mapping and filter definitions
#define LOW_DIFF 0
#define HIGH_DIFF 50
#define filterWeight 0.3f // 0.0f to 1.0f - higher value = more smoothing
float lastProx = 0;
Servo myservo;
int breading = 0; //variable to store the initial value of the proximity sensor
int pos = 0;    // variable to store the servo position

// the electrode to monitor
#define ELECTRODE 0
  
void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo
  pinMode(LED_BUILTIN, OUTPUT);
  analogWrite(LED_BUILTIN, 0);

  Serial.begin(9600);
  //while(!Serial);  
  if(!MPR121.begin(0x5A)){ 
    Serial.println("error setting up MPR121");  
    switch(MPR121.getError()){
      case NO_ERROR:
        Serial.println("no error");
        break;  
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;      
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;      
    }
    while(1);
  }

  // slow down some of the MPR121 baseline filtering to avoid 
  // filtering out slow hand movements
  MPR121.setRegister(MPR121_NHDF, 0x01); //noise half delta (falling)
  MPR121.setRegister(MPR121_FDLF, 0x3F); //filter delay limit (falling)
  MPR121.updateAll();
  breading = MPR121.getBaselineData(ELECTRODE)-MPR121.getFilteredData(ELECTRODE);
}

void loop() {

  // update all of the data from the MPR121
  MPR121.updateAll();

  // read the difference between the measured baseline and the measured continuous data
  int reading = MPR121.getBaselineData(ELECTRODE)-MPR121.getFilteredData(ELECTRODE);
  
  // print out the reading value for debug
  Serial.println(reading); 

  //if the human hand approaching the cat hand, the value of the proximity sensor changes, if the delta more than 3, than the cat hand draw back.
  if ((reading - breading) > 3)
  // constrain the reading between our low and high mapping values
 {   unsigned int prox = constrain(reading, LOW_DIFF, HIGH_DIFF);
    for (pos = 0; pos <= 50; pos += 1) { // goes from 0 degrees to 50 degrees    draw back the cat hand
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(6);                       // waits 6ms for the servo to reach the position
  }
    delay(50);                       // waits 50ms  
  for (pos = 40; pos >= 0; pos -= 1) { // goes from 40 degrees to 0 degrees    put out the cat hand again
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 10ms for the servo to reach the position
  }
 }
}
