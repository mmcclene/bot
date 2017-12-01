/* Sweep

 modified 1 Dec 2017
 by Eamonn Shirey

 Code designed for use with the Standard Gripper Kit B with an HS-422 Actuator.
 Take care to adjust the open/close settings for each setup

 Serial inputs include "open", "close", and actuator angles

 Black - Grnd
 Yellow - Signal
 Red = Power
 
*/

#include <Servo.h>
#include <ctype.h>

Servo myservo;   // create servo object to control a servo
int del = 10;    // Delay between accepting new commands (ms)
int open = 120;  // Actuator angle defined as "open"
int close = 180; // Actuator angle defined as "closed"



void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
}

void opengrip() {
  myservo.write(120);
}
void closegrip() {
  myservo.write(180);
}
int setgrip(int val) {
  myservo.write(val);
  return val;
}

void loop() {

    String state = "open";
    opengrip();
    while(true)  {
      String sd = Serial.readString();
      Serial.println(sd);
      if(sd == "close") {
        closegrip();
      }
      if(sd == "open") {
        opengrip();
      }
      Serial.println(sd.toInt());
      if(sd.toInt() != 0) {
        setgrip(sd.toInt());
      
      }
      delay(del);
    }

 
}

////180 closes hard
//closegrip();
//delay(1000);
//opengrip();
//delay(1000);
