//rover_controller_z_docs

/*****  NOTES ON UDP_SEND_STATUS_PACKET_TO_OCU ************************
void udp_send_status_packet_to_ocu(void)
{
  // send a reply, to the IP address and port that sent us the packet we received
  //THIS WORKS FOR SENDING A RESPONSE TO PYTHON UDP!!! 
  //Yes, do multiple Upd.writes of CHARS between begin and endPacket append.
  //If you udp.write an array (probably one that is not a string, with an end of string terminator)
  // it will FAIL with no error on this side.  You will get the 
  // "No Sub Status Packet Received" message on the OCU and NOT know why you lost comms.
  // eg. replyBuffer[] = "ack"; udp.write(replyBuffer); WORKS.
  // but my_array[]={1,2,3}; udp.write(my_array); seems to NOT WORK with no error message.
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  for (int x=0; x<16 ; x++)  
  {                         
    Udp.write(statusBuffer[x]); //unsigned chars.  ok to write into same packet! 
  }
  
  for (int x=0; x<16; x++)
  {
    Udp.write(piltStatusBuffer[x]);
  }
  Udp.write(messageBuffer); //const signed chars.  ok to write into same packet!
  Udp.endPacket();
  messageBuffer='\0';
}
****************************************************/

/*
1/5/15
Works on Arduino Ethernet (just checking).
Have rebuilt with W5100 Drivers so be ready to switch for W5200
1/4/15
WORKS WORKS WORKS!!!
DOES THE RIGHT THING either by planned un unplanned trip of WDT!
Still can't do with a bootloader, but otherwise looks great! HALLELUJAH!!!

1/3/14
Loaded code directly using AVRISPmk2. (no more bootloader) Works all the time!!!
Introduced WDT code; no bootloader, so no bootloader / wdt crash.
Code RUNS!!! As long as wdt_reset()s are going, things look great. But still crashes 
HARD when you do something that should trip the WDT.  After that, you have to 
POWER DOWN EVERTYTHING, and DISCONNECT ETHERNET (!!!!!!!) just to get ethernet 
talking again.  Shit.

Installeing the bootloader again and loading the code via serial port, seems to run, but 
then a WDT condition cripples the Arduino.  Constant flashing of blinky light maybe 5 Hz.
Can't upload code. Looks like it turns on a too-short WDT timeout that keeps tripping.
NOT EVEN RE-installing bootloader helps (suggests replacing bootloader does NOT wipe 
code that is above it).  Only solution is to load some code via the programmer that is 
big enough to wipe BOTH bootloader AND at least some of the begionning of the bad code.  
That stops the too-fast blinky-flash, presumably killing the loading of the too-short
WDT timeout.  Then you can reinstall the normal bootloader and re-load and run code as 
normal via the USB serial.  

HOW TO EXAMINE THE WST REGISTERS AFTER THE BAD CODE LOADS?
MAYBE THEN SEE WHAT IS BEING GOOFED UP AND CORRECT IT.
AVR STUDIO?

1/2/15
Crashed after 1.97 million loops.  Lots longer than previous tries where it crashes 
after 60K or 200K or so loops.  I reset atmega, and it starts again.
NEVER got a crash running just the minimal ethernet code on the same hardware.  
Definitely a bug that casuses a mem leak or something like heap/stack crash.
Trying again.
12/26/14
Using Pro-Mini with W5200 instead of Arduino Ethernet (with W5100) the control loop 
ticks along at approx 55 Hz!!!  Seems to work fine!
Currently W5200 files. SWAP W5200 and W5100 files in Arduino/Libraries/Ethernet
10/13/14
Added keyboard codes for the arrow keys to move in the camera menu.
WORKS!!!!! Works VERY WELL. Far more intuitive now. Still needs a way in Python to 
avoid multiple key pressed events when intending a single key press. Still can 
over-run a menu choice or toggled feature like record or menu. But much better.

Added 8 speed levels for zooming in and out.
Currently using levels 2, 4, 7.
w zooms wide (out) at 2
t zooms tele (in) at 2
z zooms wide (out) at 4
Z zooms tele (in) at 4
W zooms wide at 7
T zooms tele at 7

Added new variable int cameraPoweredUp=0;
Turning the camera OFF with 'o' sets cameraPoweredUp to 0; Camera commands that loop 
while waiting for camera feedback and would endless loop and hang the rduino 
if the camera is off will not be run. 

Turning the camera ON with 'O' sets cameraPoweredUp to 1, and enables those commands
again.  Not as good as a watchdog timer, but it will work pretty well to eliminate 
the hangs from a careless command with an off camera.

10-13-14
***MUST*** have original wiznet w5100.h and w5100.cpp in arduino/librearies/Ethernet/utility for 
the Arduino Ethernet board to work. The w5200 surfer tim patch *DOES NOT* WORK with the W5100 
on the Arduino Ethernet. 

7/14/14
Added Watchdotimer w/ 2 sec timeout.  WORKS!  Bad LANC commands force a reset instead of hanging 
the Arduino at the bottom of the ocean.
NOT WORKING.  Probably due to wild card use of WDT in Bootloaders.

5/17/14
Adding a timeout to the LANC routine.  Currently when any command is sent from Arduino to LANC, 
the Arduino code waits for a response.  If none is forthcoming because LANC doesn't recognize the 
command (or it's disconnected) the Arduino gets stuck in an endless loop waitng for response.  
Adding a timeout so that a bad command doesn't hang the Arduino.  Doesn't work; apparently because of 
tight timing beign thrown off by even doing the timeout count-down.  Need watchdog timer.

5/15/14
A few changes to the physical wiring result in the following pinout changes.

panServo.attach(18);  // attaches the servo on pin 18 (Analog4)to the servo object 
tiltServo.attach(19);

becomes:

panServo.attach(17);  // attaches the servo on pin 17 (Analog3)to the servo object 
tiltServo.attach(18); // attaches the servo on pin 18 (Analog4)to the servo object 

4/4/14
grr_all_code.ino
Use with: ocu-grr-ref-4-4-14.py

THIS IS IT (mostly)!!!
Full Motor, Servo, LANC, including full access to LANC menu to set 
AUTO/MANUAL focus so we can have full manual focus control 
(and probably exposure/iris control as well.

Only takes commands via UDP.  Menu is accessed via keypresses on 
roverControlBuffer[9].  Needs to be Udp OR'd with Serial.available.

But this totally MEETS every spec Todd has thrown at me.  Can be a lot prettier,
and will be once other things are put to bed.

3/28/14
Added code for Pin D9 as On/Off switch for Grayhill type I/O Module
to turn Video Lights on and off.
Changed variable char lancRequest to keybdRequest since it is the main parser of 
keyboard input not just for LANC but for Lights On/Off and additional TBD commands.
GRR_udp_motor_servo-lanc-2014-3-26-0400.ino
WORKS!!! with ard-ocu-udp-motor-servo-lanc-ref-2014-3-26-0400.py for approx 30Hz Loop.

This is the FULLY OPERATIONAL CONTROLLER SYSTEM FOR THE ROVER!
LANC integrated, no errors or issues.
New code using unsigned pwm with separate direction info
Use with ard-ocu-udp-ref-2014-3-25-0600.py for approx 60Hz Loop
Servos integrated, control variables changed to unsigned chars
to correct sign rollover on servos.
Approx 13K used out of 32K.  Great.
Pins used:
D3, D4, D5, D6, D7, D8, D9, D18(A4), D19(A5)
Uncommitted Pins:
A0, A1, A2, A3, Possibly D2(used by Ethernet?)
I2C bus is available!  Path for sensors, Accelerometers, compass.
Analog lines for Battery Monitor, Current, Temperature, one more.
GREAT DAY!!!!!

NEEDS!!!!!!
Safe turn on for motors, all-stop. Set motor pis as inputs with pull-down 
resistor on pwm lines
*/


/*****************************  CONTROL BUFFER ARRAY DEFINES ***************************
piltControlBuffer
0 packet start 255
1 pwmPan
2 dirPan
3 pwmTilt
4 dirTilt
5 lightsOnOff
6 panAngleIn
7 tiltAngleIn
8 packet end 254
*******************************CONTROL BUFFER ARRAY DEFINES ****************************/

/***************************   LANC COMMANDS  ************************************
//Zoom in from slowest to fastest speed
boolean ZOOM_IN_0[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW}; //28 00  //All work
boolean ZOOM_IN_1[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,LOW,LOW,LOW,HIGH,LOW}; //28 02
boolean ZOOM_IN_2[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,LOW,LOW,HIGH,LOW,LOW}; //28 04
boolean ZOOM_IN_3[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,LOW,LOW,HIGH,HIGH,LOW}; //28 06
boolean ZOOM_IN_4[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,LOW,HIGH,LOW,LOW,LOW}; //28 08
boolean ZOOM_IN_5[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,LOW,HIGH,LOW,HIGH,LOW}; //28 0A
boolean ZOOM_IN_6[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,LOW,HIGH,HIGH,LOW,LOW}; //28 0C
boolean ZOOM_IN_7[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,LOW,HIGH,HIGH,HIGH,LOW}; //28 0E

//Zoom out from slowest to fastest speed
boolean ZOOM_OUT_0[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,HIGH,LOW,LOW,LOW,LOW}; //28 10  //All work
boolean ZOOM_OUT_1[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,HIGH,LOW,LOW,HIGH,LOW}; //28 12
boolean ZOOM_OUT_2[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,HIGH,LOW,HIGH,LOW,LOW}; //28 14
boolean ZOOM_OUT_3[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,HIGH,LOW,HIGH,HIGH,LOW}; //28 16
boolean ZOOM_OUT_4[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,HIGH,HIGH,LOW,LOW,LOW}; //28 18
boolean ZOOM_OUT_5[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,HIGH,HIGH,LOW,HIGH,LOW}; //28 1A
boolean ZOOM_OUT_6[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,HIGH,HIGH,HIGH,LOW,LOW}; //28 1C
boolean ZOOM_OUT_7[] M= {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,LOW,LOW,HIGH,HIGH,HIGH,HIGH,LOW}; //28 1E

//Focus control. Camera must be switched to manual focus
boolean FOCUS_NEAR[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,HIGH,LOW,LOW,LOW,HIGH,HIGH,HIGH}; //28 47 Doesn't Work
boolean FOCUS_FAR[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,HIGH,LOW,LOW,LOW,HIGH,LOW,HIGH}; //28 45 Doesn't Work
//boolean FOCUS_AUTO[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,HIGH,LOW,LOW,LOW,LOW,LOW,HIGH}; //28 41 Doesn't Work
boolean FOCUS_MAN[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,HIGH,LOW,LOW,LOW,LOW,LOW,HIGH}; //28 41 Doesn't Work
boolean FOCUS_AUTO[] = {LOW,LOW,HIGH,LOW,HIGH,LOW,LOW,LOW,   LOW,HIGH,LOW,LOW,LOW,LOW,HIGH,HIGH}; //28 43 Someone said 43 is AF on his camera.  DOESN'T WORK
***************************************************************************************************************************/
 
