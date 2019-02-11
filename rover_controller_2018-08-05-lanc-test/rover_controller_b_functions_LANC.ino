// rover_controller_b_functions_lanc
//LANC WORKS WORKS WORKS 2017-12-24
//DO NOT ALTER OR EDIT THIS FILE WITHOUT VERY GOOD REASON!!!

void lanc_init(void)
{
  pinMode(lancPin, INPUT); //listens to the LANC line
  pinMode(cmdPin, OUTPUT); //writes to the LANC line
  Serial.println("LANC Port Initializing");
  digitalWrite(cmdPin, LOW); //set LANC line to +5V;  Ed Note: Koch says this. Maybe a 5 second high pulse turns on the camera:  didn't appear to...
  lancPowerOn(); //LANC Doc says 140ms high-low-high pulse turns on the camera.  It does.
  cameraPoweredUp=1;
  delay(1500); //Wait for camera to power up completely
  Serial.println("LANC Camera Powered On");
  Serial.println("LANC Port Initializing");
}

void lancCommand(byte headerByte, byte commandByte)
{
  boolean headerBits[8] = {0,0,0,0,0,0,0,0};
  boolean commandBits[8] = {0,0,0,0,0,0,0,0};
  byte cmdRepeatCount = 0;
  byte lanc_timeout = 0;
  //const int mask = 0x80;
  bashByteToBits(headerByte, headerBits); //break byteHeader and byteCommand into bits 
  bashByteToBits(commandByte, commandBits); //to populate BitsHeader[] and BitsCommand[];
  Serial.print("headerByte = "); 
  Serial.print(headerByte, HEX);

//  Serial.print("  headerBits = ");
//  for(int x =0; x<8; x++)
//  {
//    Serial.print(headerBits[x]);
//  }
//  Serial.println();
  Serial.print("  commandByte = ");
  Serial.print(commandByte, HEX);
//  Serial.print("  commandBits = ");
//  for(int x =7; x>-1; x--)
//  {
//    Serial.print(commandBits[x]);
//  }
  Serial.println();
  
  while (cmdRepeatCount < 5)
  {  //repeat 5 times to make sure the camera accepts the command
    while (pulseIn(lancPin, HIGH) < 5000)
    {
      //Serial.println("looping while pulsein lancpin,high<5000"); 
      //if (lanc_timeout++ == 2) //timeout to avoid endless loop if no response
      //{
      //  lanc_timeout = 0;
      //  break; 
      //}
    }
    delayMicroseconds(bitDuration);  //wait START bit duration
    //Write the 8 bits of byte 0 (bitsHeader[]) 
    //Note that the command bits have to be put out in reverse order 
    //with the least significant, right-most bit (bit 0) first
    for (int i=0; i<8; i++)
    {
       digitalWrite(cmdPin, headerBits[i]);  //Write bits. 
       delayMicroseconds(bitDuration); 
    }
 
    //Byte 0 is written now put LANC line back to +5V
    digitalWrite(cmdPin, LOW);
    delayMicroseconds(10); //make sure to be in the stop bit before byte 1
    
    while (digitalRead(lancPin))
    //while (digitalRead(lancPin) && (lanc_timeout++ < lanc_max_timeout))
    {  //Loop as long as the LANC line is +5V during the stop bit
    }
  
    //0V after the previous stop bit means the START bit of Byte 1 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //Write the 8 bits of Byte 1 (bitsCommand[])
    //Note that the command bits have to be put out in reverse order 
    //with the least significant, right-most bit (bit 0) first
    for (int i=0; i<8; i++)
    {
      digitalWrite(cmdPin, commandBits[i]);  //Write bits 
      delayMicroseconds(bitDuration);
    }
 
    //Byte 1 is written now put LANC line back to +5V
    digitalWrite(cmdPin, LOW); 
    cmdRepeatCount++;  //increase repeat count by 1
    if (lanc_timeout >=lanc_max_timeout) break;
  }//While cmdRepeatCount < 5}
}

//Take a byte and return an array[8] of its bits
void bashByteToBits(byte inByte, boolean bitsArray[])
{
  const int mask = 0x01;
  /* DIAG
  Serial.print("In BB2B, inByte HEX =");
  Serial.print(inByte, HEX);
  Serial.print(" inByte BIN =");
  Serial.println(inByte, BIN);
  */
  for (int c=0; c<8; c++)
  {
    bitsArray[c] =(inByte&mask);
    /* DIAG
    Serial.print(hexByte&mask);
    Serial.print("bit ");
    Serial.print(c);
    Serial.print(" = ");
    Serial.println(bitsArray[c]);
    */
    inByte=inByte >> 1; //right-shift inByte
  }
/*  DIAG
  for (int c=7; c>-1; c--)
  {
     Serial.print(bitsArray[c]);
  } */
  Serial.println();
  return;
}

byte getHexByte(void)
{
  error=0; //error must stay 0 throughout byte build or procedure aborts
  //Serial.println("Get with readBytes ");
  //Serial.println("Get with Serial.read ");
  //Serial.println("Waiting for first Nibble");
    //GET NIBBLE 0
  while (Serial.available() == 0)
  {}
  ////////////////// Get FIRST Hex Nibble ///////////////////////////
  Serial.readBytes(inChar,1);
  if((inChar[0]>='0') && (inChar[0]<='9')){}
  else if((inChar[0]>='A') && (inChar[0]<='F')){}
  else if((inChar[0]>='a') && (inChar[0]<='f')){}
  else {
    error=1; //not a Hex char.  Start over with new FIRST nibble.
    Serial.println("NOT LEGIT ASCII HEX CHAR! TRY AGAIN");
  }
  if (error==0) //IF first nibble Valid ASCII HEX Char, 0-9, a-f, A-F
  {              
    inNibble[0]=atoHexNibble(inChar, 0);  //Convert to most sig nibble...
    //Serial.print("InNibble[0] = ");
    Serial.print(inNibble[0], HEX);

    //...And proceed to get NIBBLE 1
    ////////////////// Get SECOND Hex Nibble ///////////////////////////
    while (Serial.available() == 0)
    {}
    Serial.readBytes(inChar,1);
    if((inChar[0]>='0') && (inChar[0]<='9')){}
    else if((inChar[0]>='A') && (inChar[0]<='F')){}
    else if((inChar[0]>='a') && (inChar[0]<='f')){}
    else {
      error=1; //not a Hex char. Start over with new FIRST nibble.
      Serial.println("NOT LEGIT ASCII HEX CHAR  TRY AGAIN");
    }
    if (error==0) //IF SECOND nibble Valid ASCII HEX Char, 0-9, a-f, A-F
    {
      inNibble[1]=atoHexNibble(inChar, 0); //Convert to least sig nibble...
      //Serial.print("InNibble[1] = ");
      Serial.print(inNibble[1], HEX);

      /////////// WAIT FOR <CR> THEN BUILD BYTE FROM NIBBLES //////////      
      while (Serial.available() == 0)
      {}
      Serial.readBytes(inChar,1);
      if((inChar[0]==10)||(inChar[0]==13))
      {   //IF <CR/LF>
        //...Proceed to build the BYTE
        hexByte=inNibble[0]*16 + inNibble[1]; 
        Serial.println();
        //Serial.print("hexByte ascii to DEC = ");
        //Serial.print(hexByte, DEC);  
        Serial.print("HEX = ");
        Serial.print(hexByte, HEX);
        Serial.print("  BIN = "); 
        Serial.print(hexByte, BIN);  
        Serial.println();
      }
      else //If no <CR>, (more keys entered, anything) start over. 
      {
        Serial.println("BYTE ENTRY ABORTED");
      }
      ///  BYTE SUCCESSFULLY BUILT! ///////////////
      
      showBitsInByte(hexByte);
      //DISPLAY THE BIT PATTERN OF THE BYTE
    }
  }
  return(hexByte);
}

byte atoHexNibble(char inChar[], int offset)
{
  byte inByte=0;
  if((inChar[offset]>='0') && (inChar[offset]<='9')){
       inByte=inChar[offset]-48;
  }
  else if((inChar[offset]>='A') && (inChar[offset]<='F')){
      inByte=inChar[offset]-'A' +10; //Since A = 10 DEC
  }
  else if((inChar[offset]>='a') && (inChar[offset]<='f')){
      inByte=inChar[offset]-'a' +10;
  }
  else {
    inByte=255;
  }
  return(inByte);
}

void showBitsInByte(byte Byte){
//DISPLAY THE BIT PATTERN OF THE BYTE
  Serial.println("showBitsInByte()");
  boolean bitsArray[8]={0,0,0,0,0,0,0,0};
  const int mask = 0x01;
  for (int c=0; c<8; c++)
  {
    bitsArray[c] =(Byte&mask);
    Serial.print("bit ");
    Serial.print(c);
    Serial.print(" = ");
    Serial.println(bitsArray[c]);
    Byte=Byte >> 1;
  }
  for (int c=0; c<8; c++)
  {
     Serial.print(bitsArray[c]);
  }
  Serial.println();
}

void lancPowerOn(void) //WORKS www.boehmel.de/lanc.htm 
{
  digitalWrite(cmdPin, HIGH);
  delay(200);
  digitalWrite(cmdPin, LOW);
  cameraPoweredUp=1;
}

/* RAW HEX LANC COMMAND builder: for diag
void rawHexCommandMenu(void)
{
  Serial.println("Enter Header Byte");
  messageBuffer="Enter Header Byte";
  byte headerByte=getHexByte();
  while(1)
  {
    Serial.print("Header Byte =");
    Serial.println(headerByte, HEX);
    Serial.println("Enter Command Byte (FF to exit)");
    messageBuffer="Enter Command Byte (FF to exit)";
    byte commandByte=getHexByte();
    if (commandByte==0xFF) break;
    lancCommand(headerByte,commandByte);
  }
  return;
}
*/

