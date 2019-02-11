//rover_controller_b_functions_pilt

void serial_tx_piltCommandPacket(void){
  Serial.write(piltCommandPacket, 16);
}

int serial_rx_piltStatusPacket(void)
{
  newPacketFlag = 0;
  flush_packet();
  if (Serial.available()) // if there are one or more chars...
  {
    char inChar = Serial.read(); // get the first char...
    if (inChar == startPacketByte) // is it startPacketByte?
    {
      Serial.readBytesUntil(endPacketByte, piltStatusPacket, 16); //if yes, ... 
      for (int x=0; x<14; x++)  // get chars til endPacketByte or get the next 16 chars
      {
//        Serial.print(piltStatusPacket[x]); // but packet[0] is x_pwm NOT startPacketByte
      }
      Serial.println();
      Serial.write(piltStatusPacket, 16);
      Serial.println();
      newPacketFlag = 1;
    }
  }
  return newPacketFlag;
}

void flush_packet(void){
  for (byte x=0; x<packetSizePilt; x++){piltStatusPacket[x] = 0;}
}


void build_piltCommandPacketAscii(void) //ASCII VISIBLE
{
  piltCommandPacket[0]= startPacketByte;
  piltCommandPacket[1]= 65;
  piltCommandPacket[2]= 66;
  piltCommandPacket[3]= 67;
  piltCommandPacket[4]= 68;
  piltCommandPacket[5]= 69;
  piltCommandPacket[6]= 70;
  piltCommandPacket[7]= 71;
  piltCommandPacket[8]= 72;
  piltCommandPacket[9]= 73;
  piltCommandPacket[10]= 74; 
  piltCommandPacket[11]= 75;
  piltCommandPacket[12]= 76;
  piltCommandPacket[13]= 77;
  piltCommandPacket[14]= 78;
  piltCommandPacket[15]= endPacketByte;
}

/*
void build_piltCommandPacket(void) //real
{
  piltCommandPacket[0]= startPacketByte;
  piltCommandPacket[1]= pwmPan;
  piltCommandPacket[2]= dirPan;
  piltCommandPacket[3]= spare0;//pan_fault
  piltCommandPacket[4]= pwmTilt;
  piltCommandPacket[5]= dirTilt;
  piltCommandPacket[6]= spare1; // tilt_fault
  piltCommandPacket[7]= motorEnableFlag;
  piltCommandPacket[8]= lights;
  piltCommandPacket[9]= faultPan;
  piltCommandPacket[10]= faultTilt; 
  piltCommandPacket[11]= auxCommand;
  piltCommandPacket[12]= spare2;
  piltCommandPacket[13]= spare3;
  piltCommandPacket[14]= spare4;
  piltCommandPacket[15]= endPacketByte;
}
*/



void set_pilt_status_vars_from_piltStatusPacket(void)
{
  startPacketByte = piltStatusPacket[0];
  pwmPan = piltStatusPacket[1];
  dirPan = piltStatusPacket[2];
  anglePan = piltStatusPacket[3];
  pwmTilt = piltStatusPacket[4];
  dirTilt = piltStatusPacket[5];
  angleTilt = piltStatusPacket[6];
  motorEnableFlag = piltStatusPacket[7];
  lights = piltStatusPacket[8];
  faultPan = piltStatusPacket[9];
  faultTilt = piltStatusPacket[10];
  auxCommandAck = piltStatusPacket[11];
  spare0 = piltStatusPacket[12];
  spare1 = piltStatusPacket[13];
  spare2 = piltStatusPacket[14];
  endPacketByte = piltStatusPacket[15];
}

