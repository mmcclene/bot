/*
   Bluetooth Boom-Gripper bot.

   Goal in life...
      Follows your commnds sent magically though the air! Or from USB :)

   Written by Scott Beasley - 2015
   Free to use or modify. Enjoy.
*/

#include <SoftwareSerial.h>

// Various time delays used for driving and servo
#define TURNDELAY 475
#define TURNDELAY45 235

// Pololu servo controller commands
#define SETSPEED 0x01
#define SETPOS 0x04

/*
   Globals area.
*/

// Create the motor, servo objects to interface with
SoftwareSerial Serial_servo (10, 11);
SoftwareSerial Serial_motors (10, 12);

void setup ( )
{
   // Change the baud rates here if diffrent than below
   Serial.begin (9600); // Bluethooth connection
   Serial_servo.begin (9600);
   Serial_motors.begin (19200);

   halt ( );
   set_servo_speed (0, 10); // Set the servo movment speed
   set_servo_speed (1, 10); // to make them move a bit smoother
   home_boom ( );
   home_gripper ( );

   delay (500);
}

void loop ( )
{
   byte command = 0, val = 0;
   // The following need to stay intact during the life of the 
   // session so they are made 'static'. This keeps us from 
   // having to have them as globals
   static byte dist = 1; // Start with short moves
   static byte speed = 1; // slow speed
   static byte speed_offset = 40;
   static byte turn_style = 1; // and with hard turns
   static byte boom_val = 60;
   static byte gripper_val = 60;

   if (Serial.available ( ) > 0) {
      // read the incoming command byte
      command = Serial.read ( );
   }

   switch (command) {
      case 'w':
         go_forward (speed_offset);
         //Serial.println ("Going Forward");

         if (dist == 1) {
            delay (750);
            halt ( );
         } else if (dist == 2) {
            delay (1250);
            halt ( );
         }

         break;

      case 'z':
         go_backward (speed_offset);
         //Serial.println ("Going Backwards");

         if (dist == 1) {
            delay (750);
            halt ( );
         } else if (dist == 2) {
            delay (1250);
            halt ( );
         }

         break;

      case 'a':
         go_left (speed_offset, turn_style);
         delay (TURNDELAY);
         halt ( );
         //Serial.println ("Turning Left");
         break;

      case 's':
         go_right (speed_offset, turn_style);
         delay (TURNDELAY);
         halt ( );
         //Serial.println ("Turning Right");
         break;

      case 'q':
         go_left (speed_offset, turn_style);
         delay (TURNDELAY45);
         halt ( );
         //Serial.println ("Turning Left");
         break;

      case 'e':
         go_right (speed_offset, turn_style);
         delay (TURNDELAY45);
         halt ( );
         //Serial.println ("Turning Right");
         break;

      case 'h':
         halt ( );
         //Serial.println ("Halting");
         break;

      case '>':
         // Gripper X move sends servo set value
         val = Serial.read ( );
         gripper_val = val;
	 
         // We limit the value to real movemovent limits of the setup
         set_gripper (val);
         //Serial.println ("GripperX");
         break;

      case '^':
         // Gripper Y move sends servo set value
         val = Serial.read ( );
         boom_val = val;
	 
         // We limit the value to real movemovent limits of the setup
         set_boom (val);
         //Serial.println ("GripperY");
         break;

      case 'c':
	 gripper_val = 60;
	 boom_val = 60;
         home_boom ( );
         home_gripper ( );
	 
         //Serial.println ("GripperHome");
         break;

      case 'v':
         // Set the spped the bot will move; 1 = slowest, 3 = fastest
         speed = Serial.read ( );

         if (speed == 1) {
            speed_offset = 40;    // Slow
         } else if (speed == 2) {
            speed_offset = 20;    // Med
         } else if (speed == 3) {
            speed_offset = 0;     // Fast!
         }

         //Serial.println ("Set speed");
         break;

      case 'd':
         // Set how long the bot will move; 1 = short, 2 medium,
         // 3 = continuous
         dist = Serial.read ( );
         //Serial.println ("Move distance set");
         break;

      case 't':
         // Set the way the bot does turns, one wheel (soft) or two (hard)
         turn_style = Serial.read ( );

         //Serial.println ("Turn style");
         break;

      case '+': // Speed set from phone
         speed++;
         if (speed >= 4)
            speed = 1;
	     
         if (speed == 1) {
            speed_offset = 40;    // Slow
         } else if (speed == 2) {
            speed_offset = 20;    // Med
         } else if (speed == 3) {
            speed_offset = 0;     // Fast!
         }
	     
         break;

      case '-': // Dist moved from phone
         dist++;
	  
         if (dist >= 4)
            dist = 1;
         //Serial.println ("Move distance set");
         break;

      case ',': // Open Gripper from phone
         gripper_val += 10;
	  
         if (gripper_val >= 179)
            gripper_val = 179;

         set_gripper (gripper_val);

         //Serial.println ("Move distance set");
         break;

      case '.': // Close Gripper from phone
         gripper_val -= 10;
	  
         if (gripper_val <= 0)
            gripper_val = 0;

         set_gripper (gripper_val);

         //Serial.println ("Move distance set");
         break;

      case '0': // Lower boom from phone
         boom_val += 10;
	  
         if (boom_val >= 179)
            boom_val = 179;

         set_boom (boom_val);

         //Serial.println ("Boom set");
         break;

      case '1': // Raise boom from phone
         boom_val -= 10;
	  
         if (boom_val <= 0)
            boom_val = 0;

         set_boom (boom_val);

         //Serial.println ("Boom set");
         break;

       case 255: // Sent after all gripper commands
          Serial.flush ( );
          break;
   }

   Serial.flush ( );
   delay(125);
}

void go_forward (byte speed_offset)
{
   //Serial.println ("Going forward...");
   Serial_motors.write ((byte)1 + speed_offset);
   Serial_motors.write ((byte)128 + speed_offset);
}

void go_backward (byte speed_offset)
{
   //Serial.println ("Going backward...");
   Serial_motors.write ((byte)127 - speed_offset);
   Serial_motors.write ((byte)255 - speed_offset);
}

void go_left (byte speed_offset, byte turn_style)
{
   //Serial.println ("Going left...");
   if (turn_style == 1)
      Serial_motors.write ((byte)1 + speed_offset);
   else
      Serial_motors.write ((byte)64); // Turn off the left motor (soft turn)

   Serial_motors.write ((byte)255 - speed_offset);
}

void go_right (byte speed_offset, byte turn_style)
{
   //Serial.println ("Going right...");

   Serial_motors.write ((byte)127 - speed_offset);

   if (turn_style == 1)
      Serial_motors.write ((byte)128 + speed_offset);
   else
      Serial_motors.write ((byte)192); // Turn off the right motor (soft turn)
}

void halt ( )
{
   //Serial.println ("Halt!");
   Serial_motors.write ((byte)0);
}

// Set a servos position
//   servo_id is the servo number (0-7)
//   angle is the abs position from 500 to 5500
void move_servo (int servo_id, int angle)
{
   unsigned char buff[6] = {0x80, 0x01, SETPOS, 0x00, 0x00, 0x00};

   unsigned int temp;
   unsigned char data_hi, data_low;

   temp = angle & 0x1f80;
   data_hi = temp >> 7;
   data_low = angle & 0x7f;

   buff[3] = servo_id; //servo number
   buff[4] = data_hi; //data1
   buff[5] = data_low; //data2

   Serial_servo.write (buff, 6);
   Serial_servo.flush ( );
}

void set_servo_speed (int servo_id, int speed)
{
   unsigned char buff[5] = {0x80, 0x01, SETSPEED, 0x00, 0x00};
   if (speed > 127)
      speed = 127;

   buff[3] = lowByte (servo_id);
   buff[4] = lowByte (speed);
   Serial_servo.write (buff, 5);
   Serial_servo.flush ( );
}

void set_boom (byte angle)
{
   move_servo (0, map (angle, 0, 179, 2500, 4000));
}

void set_gripper (byte angle)
{
   move_servo (1, map (angle, 0, 179, 2500, 4000));
}

void home_boom ( )
{
   move_servo (0, 3000);
}

void home_gripper ( )
{
   move_servo (1, 3000);
}
