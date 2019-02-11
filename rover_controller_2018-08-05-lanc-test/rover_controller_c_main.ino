//rover_controller_c_main.ino

void setup()
{
  Serial.begin(115200);
  pinMode(dirLeftPin, OUTPUT);  //Tractor Motors
  pinMode(dirRightPin, OUTPUT); 
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  lanc_init();
  print_initial_message();
  //Serial.println("End of setup start of loop");
}

void loop()
{
  //UDP DATA HANDLER If there's data available, read a packet
  int packetSize = Udp.parsePacket();
  //Serial.print("packetSize = ");
  //Serial.println(packetSize);
  if(packetSize)  // if a packet, read the packet into packetBufffer
  {
    //Serial.println("got packet");
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    for(int x=0; x<16; x++)
    {
       roverControlBuffer[x]=packetBuffer[x]; 
    }
    //Serial.println("packet in roverControlBuffer");
    set_rover_vars_from_roverControlBuffer();
    //Serial.println("set_rover_vars_from_roverControlBuffer done");  
    set_tractor_motor_pwms();
    //Serial.println("set_tractor_motor_pwms() done"); 
    for (int x=0; x<16; x++)
    {
      statusBuffer[x] = roverControlBuffer[x];
    }
    statusBuffer[13] = 70;
    statusBuffer[14] = 80;
    statusBuffer[15] = 90;
/*    for (int x=0; x<16; x++)
    {
      Serial.print(statusBuffer[x]);
      Serial.print('\t');
    }
    for (int x=0; x<8; x++)
    {
      Serial.print(piltControlBuffer[x]);
      Serial.print('\t');
    }
    Serial.println(""); */
    if (inKey > 0)
    {
      Serial.print("INKEY = ");
      Serial.println(inKey);
      parse_inKey();
    }
    //build_piltCommandPacket(); //REAL non-visible "byte" values
    build_piltCommandPacketAscii(); //DIAG ascii visible values for serial monitor 
    serial_tx_piltCommandPacket();
    //Serial.println("serial tx command packet done");
    flush_packet();
    serial_rx_piltStatusPacket();
    //udp_send_status_packet_to_ocu();

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    for (int x=0; x<16 ; x++)  
    {                         
      Udp.write(statusBuffer[x]); //unsigned chars.  ok to write into same packet! 
    }
    Udp.write(messageBuffer); //const signed chars.  ok to write into same packet!
    Udp.endPacket();
    messageBuffer='\0';


    
    //Serial.println("udp send status packet to ocu done");
  }
  //Serial.println("end of main loop");
}

