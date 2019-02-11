//rover_controller_a_header

/********** INCLUDES *******************************************/
#include <SPI.h> // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h> // UDP library from: bjoern@cs.stanford.edu 12/30/2008
/********** INCLUDES END *******************************************/

/********** DEFINES ************************************/
//These worked for these I/O pins, so they will work for others. Do #defines instead of vars
/********** DEFINES END ************************************/

/************** ETHERNET UDP SETUP ******************************/
// Enter a MAC address and IP address for your controller below.
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x53, 0xE1};
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip(192, 168, 1, 99);
unsigned int localPort = 60615;      // local port to listen on
unsigned char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
//char  replyBuffer[12] = "";   // buffer to hold a string to send back. NOT USED???
EthernetUDP Udp; // An EthernetUDP instance to let us send and receive packets over UDP
/****************** ETHERNET UDP SETUP END ******************************************/

/****************I/O PIN SETUP ****************************/
//Motor PWM Pin Setup
const int motorLeftPin = 5;   // Left Motor PWM
const int motorRightPin = 6;  // Right Motor PWM
const int dirLeftPin = 7;     // Left Motor Dir
const int dirRightPin = 8;    // Right Motor Dir
const int motorEnablePin = 2; // Motor Driver Enable
//LANC Pin Setup
const int cmdPin = 3;
const int lancPin = 4;
/*********I/O PIN SETUP END****************************/

/*****  MOTOR CONTROL VARIABLES INIT *****************/
unsigned char pwmMotorLeft =0;
unsigned char dirMotorLeft = 1;
unsigned char pwmMotorRight =0;
unsigned char dirMotorRight = 1;
unsigned char roverControlBuffer[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 16 bytes
unsigned char statusBuffer[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 16 bytes
char messageBuffer="\n"; //must be CONST CHAR

unsigned char inKey = '\n'; //Command Byte: from OCU packetBuffer[9]
unsigned char button = 0;
unsigned char jsHatX = 0;
unsigned char jsHatY = 0;
/*****  MOTOR CONTROL VARIABLES INIT END *****************/

/*****  PILT CONTROL VARIABLES INIT  *****************/
unsigned char startPacketByte = 123;
unsigned char endPacketByte = 125;
//unsigned char start_packet_byte = 255;
//unsigned char end_packet_byte = 254;
unsigned char pwmPan = 0;
unsigned char pwmTilt = 0;
unsigned char dirPan = 0;
unsigned char dirTilt = 0;
unsigned char anglePan = 0;
unsigned char angleTilt = 0;
unsigned char lights = 0;
unsigned char motorEnableFlag = 0; 
unsigned char newPacketFlag = 0;
unsigned char faultPan = 0;
unsigned char faultTilt = 0;
unsigned char auxCommand = 0;
unsigned char auxCommandAck = 0;
unsigned char spare0 = 0;
unsigned char spare1 = 0;
unsigned char spare2 = 0;
unsigned char spare3 = 0;
unsigned char spare4 = 0;
unsigned char piltCommandPacket[16]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char piltStatusPacket[16]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
unsigned char piltStatusBuffer[16] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char piltControlBuffer[16] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char piltLoopCount = 0;
unsigned char packetSizePilt = 16;
/*****  PILT CONTROL VARIABLES INIT END *****************/

/********  LANC COMMAND VARIABLES INIT *********************/
int cmdRepeatCount;
//int bitDuration = 104; //Duration of one LANC bit in microseconds. 
int bitDuration = 96; //Writing to the digital port takes about 8 microseconds so only 96 microseconds are left for each bit
//int inByte = '\n';
byte lanc_max_timeout =100;
//LANC commands byte 0 + byte 1

//Power On / Off
byte PWR_OFF = 0x5E; //18 5E Works
byte PWR_ON = 0x5C; //18 5C Doesn't Work. Use powerOn() func; It works.
//  Doesn't work Koch says there's no power supply from the LANC port when the camera is off. But our circuit is 
//powered independently, so not likely here.

//Start-stop video recording
byte REC = 0x33; //18 33  Works
byte REC_STOP = 0x30; //18 30
byte PHOTO = 0x39; //18 39
byte PAUSE = 0x32; //18 32  Doesn't Work

//Zoom and Focus All even multiples.  What (if anything) do the odd multiples do?  Try sometime.
byte ZOOM_IN_0 = 0x00; //28 00
byte ZOOM_IN_1 = 0x02; //28 02
byte ZOOM_IN_2 = 0x04; //28 04
byte ZOOM_IN_3 = 0x06; //28 06
byte ZOOM_IN_4 = 0x08; //28 08
byte ZOOM_IN_5 = 0x0A; //28 0A
byte ZOOM_IN_6 = 0x0C; //28 0C
byte ZOOM_IN_7 = 0x0E; //28 0E

byte ZOOM_OUT_0 = 0x10;//28 10
byte ZOOM_OUT_1 = 0x12;//28 12
byte ZOOM_OUT_2 = 0x14;//28 14
byte ZOOM_OUT_3 = 0x16;//28 16
byte ZOOM_OUT_4 = 0x18;//28 18
byte ZOOM_OUT_5 = 0x1A;//28 1A
byte ZOOM_OUT_6 = 0x1C;//28 1C
byte ZOOM_OUT_7 = 0x1E;//28 1E

byte FOCUS_FAR = 0x45; //18 45  Works IN MANUAL MODE ONLY
byte FOCUS_NEAR = 0x47; //18 47  Works IN MANUAL MODE ONLY

/*********  NEW COMMANDS THAT WORK! **************************/
byte MENU = 0x9A; //18 9A WORKS!!! Displays Initial Menu Screen
byte MENU_RIGHT_NEXT = 0xC2; //18 C2: AFTER 9A, Moves right one menu choice!
byte MENU_LEFT_PREV = 0xC4; //18 C4: AFTER 9A, Moves LEFT one menu choice!
byte MENU_UP = 0x84; //18 84 Moves UP one menu choice!
byte MENU_DOWN = 0x86; //18 86 Moves DOWN one menu choice!
byte EXECUTE = 0xA2; //18 A2 PRESSES THE MENU BUTTON YOU GOT TO WITH 9A, C2, C4, 84, 8647
/********  LANC COMMAND VARIABLES INIT END *****************/

/***************************** GET_HEX_BYTE VARIABLE INIT ********************************/
char inChar[2]={'z','z'};
byte inNibble[2] = {250,251};
byte hexByte = 0;
byte atoHexNibble(char inChar[], int offset);
int error=0;
/***************************** GET_HEX_BYTE VARIABLES INIT END ********************************/

/********  MISC VARIABLES INIT *****************/
boolean ON = 1; //For turning I/O Modules and attached devices on or off
boolean OFF = 0; //For turning I/O Modules and attached devices on or off
unsigned char motors_enabled_flag = 0;
int cameraPoweredUp=0;
unsigned char MOTORS_ON = 1;
unsigned char MOTORS_OFF = 0;

/********  MISC VARIABLES INIT END *****************/

/*********  FUNCTION DECLARATIONS ******************/
void parse_inKey(void);
void lancPowerOn(void);
void lancCommand(byte headerByte, byte commandByte);
int setMotorPwm(int motorPin, int newPwm);
int setMotorDir(int dirPin, int newDir);
boolean lightsOn(void);  //Returns ON, Status of Lights
boolean lightsOff(void); //Returns OFF, Status of Lights
void showBitsInByte(byte Byte);
byte atoHexNibble(char inChar[], int offset);
byte getHexByte(void);
void bashByteToBits(byte inByte, boolean bitsArray[]);
//void rawHexCommandMenu(void); // for making new LANC codes fordevelopment and diag
//void displayCommandArray(boolean commandArray[]);
void showBitsInByte(byte Byte);
void showBitsInWord(byte Byte0, byte Byte1);
void sendUdpMessage(char *messageBuffer);
void enableMotors(void);
void disableMotors(void);
void lanc_init(void);
void set_tractor_motor_pwms(void);
void print_initial_message(void);
void build_piltCommandPacket(void);
void serial_send_pilt_command_packet(void);
void serial_receive_pilt_status_packet(void);
void set_pilt_status_vars_from_piltStatusPacket(void);
void add_pilt_status_to_status_packet(void);
void set_rover_vars_from_roverControlBuffer(void);
void serial_tx_piltCommandPacket(void);
int serial_rx_piltStatusPacket(void);
//int get_serial_inPacket(void);
void build_piltCommandPacket(void);
void build_piltCommandPacketAscii(void);
void flush_packet(void);
/*********  FUNCTION DECLARATIONS END  *************/
void udp_send_status_packet_to_ocu(void);
