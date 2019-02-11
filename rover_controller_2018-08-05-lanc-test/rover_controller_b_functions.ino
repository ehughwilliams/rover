//rover_controller_b_functions

/*********************************FUNCTIONS****************************************/

void parse_inKey(void)
{      
  Serial.print("INSIDE PARSE_INKEY INKEY = ");
  Serial.println(inKey);
  switch (inKey)
  {
    /////////////////  LANC VIDEO CONTROL COMMANDS //////////////
    case ' ':    
      disableMotors();
      break;
    case 'E':  
      enableMotors();
      break;
    case 'P':    
      if(cameraPoweredUp == 1) lancCommand(0x18, PHOTO);  //Doesn't work
      break;
    case 'r':  //To confirm if RECORDING, select 'M' for Menu. REC/STBY status is displayed
      if(cameraPoweredUp == 1) lancCommand(0x18, REC);//18 33 Works
      break;
    case 'R':    
      if(cameraPoweredUp == 1) lancCommand(0x18, REC);//18 33 Works
      break;
    case 'z':
      if(cameraPoweredUp == 1) lancCommand(0x28, ZOOM_OUT_2);
      break;
    case 'w':    
      if(cameraPoweredUp == 1) lancCommand(0x28, ZOOM_OUT_2);
      break;
    case 'Z':
      if(cameraPoweredUp == 1) lancCommand(0x28, ZOOM_IN_2);
    case 't':    
      if(cameraPoweredUp == 1) lancCommand(0x28, ZOOM_IN_2);
      break;
    case 'W':    
      if(cameraPoweredUp == 1) lancCommand(0x28, ZOOM_OUT_7);
      break;
    case 'T':    
      if(cameraPoweredUp == 1) lancCommand(0x28, ZOOM_IN_7);
      break;

    case 'f': 
      if(cameraPoweredUp == 1) lancCommand(0x28, FOCUS_NEAR); //WORKS, IN MANUAL FOCUS MODE ONLY   
      break;
    case 'F': 
      if(cameraPoweredUp == 1) lancCommand(0x28, FOCUS_FAR);  //WORKS, IN MANUAL FOCUS MODE ONLY   
      break;

    //////////  LANC MENU COMMANDS /////////////
    case 'M': 
      if(cameraPoweredUp == 1) lancCommand(0x18, MENU);  //WORKS   
      break;
    case 'm': 
      if(cameraPoweredUp == 1) lancCommand(0x18, MENU);  //WORKS   
      break;
    case 13: 
      if(cameraPoweredUp == 1) lancCommand(0x18, EXECUTE);  //WORKS, IN MANUAL FOCUS MODE ONLY   
      break;
    case 128: //pygame up_arrow 273 - 145 (to get into unsigned char range)
      if(cameraPoweredUp == 1) lancCommand(0x18, MENU_UP);   
      break;
    case 129: //pygame down_arrow 274 - 145 (to get into unsigned char range)   
      if(cameraPoweredUp == 1) lancCommand(0x18, MENU_DOWN); //18 5E Works
      break;
    case 130: //pygame right_arrow 275 - 145 (to get into unsigned char range)    
      if(cameraPoweredUp == 1) lancCommand(0x18, MENU_RIGHT_NEXT);
      break;
    case 131: //pygame left_arrow 276 - 145 (to get into unsigned char range)    
      if(cameraPoweredUp == 1) lancCommand(0x18, MENU_LEFT_PREV);
      break;
    case 'O': 
      lancPowerOn();  
      cameraPoweredUp=1;   
      break;
    case 'o':    
      if(cameraPoweredUp == 1) lancCommand(0x18, PWR_OFF); //18 5E Works
      cameraPoweredUp=0;
      break;
    case 'L':    
      lights = lightsOn();
      break;
    case 'l':    
      lights = lightsOff();
      break;
    case 'Q':
      while(1);
      break;
  /*  case 'X': //make this set the Header Byte so it doesn't need to be re-entered each time. 
      rawHexCommandMenu(); //Good for diag,testing new codes. BAD for End User Code!
      break;
    case 'x':    
      rawHexCommandMenu();
      break;  */
    /*case 'p':    
      lancCommand(PAUSE);  //Doesn't work
      break;
    case 'a': 
      lancCommand(FOCUS_AUTO);   //Doesn't work 
      break;
    case 'm': 
      lancCommand(FOCUS_MAN);  //Doesn't work   
      break;  */
    default:
      messageBuffer="Invalid Command";
      //Serial.println("Invalid Command");
  }
  Serial.println(inKey);
}

void set_rover_vars_from_roverControlBuffer(void)
{
  pwmMotorLeft = roverControlBuffer[0];
  dirMotorLeft = roverControlBuffer[1];
  pwmMotorRight = roverControlBuffer[2];
  dirMotorRight = roverControlBuffer[3];
  pwmPan = roverControlBuffer[4]; //to send to PILT
  dirPan = roverControlBuffer[5]; //to send to PILT
  pwmTilt = roverControlBuffer[6]; //to send to PILT
  dirTilt = roverControlBuffer[7]; //to send to PILT
  button = roverControlBuffer[8];
  inKey = roverControlBuffer[9];
  jsHatX = roverControlBuffer[10];
  jsHatY = roverControlBuffer[11];
  motors_enabled_flag = roverControlBuffer[12];
  return;
}

void udp_send_status_packet_to_ocu(void)
{
  // see NOTES ON UDP_SEND_STATUS_PACKET_TO_OCU re idiosyncracies sending strings, chars, bytes
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

boolean lightsOn(void)
{
    //send lightson to pilt
    lights = ON;
    return(ON);
}

boolean lightsOff(void)
{
    //send lightsoff to pilt
    lights = OFF;
    return(OFF);
}

void print_initial_message(void)
{
  Serial.println("REEF ROVER Serial Port Initialized");
  Serial.println("REEF ROVER CONTROLLER" );  
  Serial.println("grrv2_rover_controller_2017-12-02.ino" );
  Serial.println("Use with grrv2_ocu_2017-12-02.py" );
  //Ethernet.begin(mac,ip);
 // Udp.begin(localPort);
  Serial.print("Rover is at ");
  Serial.println(Ethernet.localIP()); 
} 

/* USEFUL BUT NOT BEING USED:
//Currently building packet by in-line code from roverControlBuffer[] and messageBuffer[]. 
//Maybe move back to this function some day.
void sendUdpMessage(char messageBuffer[])
{
            Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            Udp.write(messageBuffer);
            Udp.endPacket();
            return;
}
*/
/******************************  FUNCTIONS END **************************************/

