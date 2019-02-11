/*  TRACTOR CODE -- IP ADDR 192.168.1.150 PORT 8880 */

/********** INCLUDES *******************************************/
#include <SPI.h> // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h> // UDP library from: bjoern@cs.stanford.edu 12/30/2008
/********** INCLUDES END *******************************************/

/************** ETHERNET UDP SETUP ******************************/
// Enter a MAC address and IP address for your controller below.
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x53, 0xE0};// MAC ADDR MUST BE UNIQUE FOR MULTI
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip(192, 168, 1, 150);
unsigned int localPort = 8880;      // local port to listen on
unsigned char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
EthernetUDP Udp; // An EthernetUDP instance to let us send and receive packets over UDP
/************* ETHERNET UDP SETUP END ***************/

/********  MISC VARIABLES INIT *****************/
unsigned char controlBuffer[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
String messageBuffer = "tractor";
/********  MISC VARIABLES INIT END *****************/

/*********  FUNCTION DECLARATIONS ******************/
//void sendUdpMessage(char *messageBuffer); //not used in stripped code
/*********  FUNCTION DECLARATIONS END  *************/
 
void setup()
{
  Serial.begin(115200);
  Serial.println("one-of-many-150-tractor-strip.ino" );
  Serial.println("Use with ocu150-strip.py" );
  
  // start the Ethernet and UDP:
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  Serial.print("Tractor is at ");
  Serial.println(Ethernet.localIP()); 
}

void loop()
{
  //**********UDP DATA HANDLER ******/
    /******* READ THE UDP PACKET INTO packetBufffer *****/ 
    int packetSize = Udp.parsePacket();
    if(packetSize)
    {

      Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
      
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      for (int i=0; i < 4; i++) {
        Serial.print(remote[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }
      Serial.print(", port ");
      Serial.print(Udp.remotePort());
      //Serial.print(", controlBuffer = ");//for diag
      for(int x=0; x<16; x++)
      {
         controlBuffer[x]=(packetBuffer[x] *2); // do 2X on data before returning to verify recv-xmit.
         //why don't I have to cast these back to bytes?...
         //Serial.print(controlBuffer[x]); //for diag
         //Serial.print(" ");
      }
      
      /************* SEND UDP STATUS PACKET BACK TO OCU *******************/
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      for (int x=0; x<16; x++)  
      {                         
        Udp.write(controlBuffer[x]); //unsigned chars.  ok to write into same packet! 
      }
      //Udp.write(messageBuffer); //not working - ascii garbage at OCU.  Wrong type? bad EOF?
      Udp.endPacket();
      //messageBuffer='\0';
      /************* SEND UDP STATUS PACKET BACK TO OCU END *****************/
    }
}
