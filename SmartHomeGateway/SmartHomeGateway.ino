
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <Ethernet.h>
#include <PubSubClient.h>
#include <MQTTStruct.h>

//Content of MQTTStruct.H file is as follow: 
//typedef struct {
//  String Topic;
//  String Temp;
//  String Humidity;
//  String Smoke;
//  String CO;
//}MQTTStruct;
//The above should be placed in arduino folder named MQTTStruct

// Constructor.
// \param[in] speed The desired bit rate in bits per second
// \param[in] rxPin The pin that is used to get data from the receiver
// \param[in] txPin The pin that is used to send data to the transmitter
// \param[in] pttPin The pin that is connected to the transmitter controller. It will be set HIGH to enable the transmitter (unless pttInverted is true).
// \param[in] pttInverted true if you desire the pttin to be inverted so that LOW wil enable the transmitter.
// RH_ASK(uint16_t speed = 2000, uint8_t rxPin = 11, uint8_t txPin = 12, uint8_t pttPin = 10, bool pttInverted =
RH_ASK driver(2000, 2,3,4); // ESP8266: do not use pin 11

//Ethernet MAC address
byte mac[]    = {  0x90, 0xA2, 0xDA, 0x0D, 0x11, 0x11 };
//IP address of the Arduino Ethernet Card
IPAddress ip(192, 168, 1, 151);
//IP address of MQTT server
byte server[] = { 192, 168, 1, 150 };

//We should never arrive in the callback function as arduino is not subscribed to any topics on the MQTT server, it is only publishing. 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("");
}

EthernetClient ethClient;
PubSubClient client(ethClient);



void setup()
{
    Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");
    client.setServer(server, 1883);
    client.setCallback(callback);
    Ethernet.begin(mac, ip);
    delay(1500);
}

void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);
    String askMessage; 
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
        int i;
        for (i=0;i<buflen;i++)               
        {
        askMessage += char(buf[i]);  
        } 
    char Topic[15];
    char Values[10];

    MQTTStruct MQTTItemsTotransmit;
    MQTTItemsTotransmit = getMQTTvals(askMessage);

    String TempTopic = MQTTItemsTotransmit.Location +"/Temp";
    String HumidityTopic = MQTTItemsTotransmit.Location +"/Hum";
    String SmokeTopic = MQTTItemsTotransmit.Location +"/Smoke";
    String COTopic = MQTTItemsTotransmit.Location +"/CO";
    String Temp = MQTTItemsTotransmit.Temp;
    String Humidity = MQTTItemsTotransmit.Humidity;
    String Smoke = MQTTItemsTotransmit.Smoke; 
    String CO = MQTTItemsTotransmit.CO;

    TempTopic.toCharArray(Topic,15);
    Temp.toCharArray(Values,10);
    sendMQTTMessage(Topic,Values);
    delay(100);
    HumidityTopic.toCharArray(Topic,15);
    Humidity.toCharArray(Values,10);
    sendMQTTMessage(Topic,Values);
    delay(100);
    SmokeTopic.toCharArray(Topic,15);
    Smoke.toCharArray(Values,10);
    sendMQTTMessage(Topic,Values);
    delay(100);
    COTopic.toCharArray(Topic,15);
    CO.toCharArray(Values,10);
    sendMQTTMessage(Topic,Values);
    }
}

//will return the value of the received message
//can be used to submit via MQTT as value
MQTTStruct getMQTTvals(String rcvdMessage){
  MQTTStruct val;
  uint8_t indexCutoff;
  int firstIndex = rcvdMessage.indexOf("&&"); 
  int secondIndex = rcvdMessage.indexOf("&&", firstIndex + 1); 
  int thirdIndex = rcvdMessage.indexOf("&&",secondIndex + 1);
  int fourthIndex = rcvdMessage.indexOf("&&",thirdIndex + 1); 
  val.Location = rcvdMessage.substring(0,firstIndex);
  val.Temp = rcvdMessage.substring(firstIndex +2,secondIndex);
  val.Humidity = rcvdMessage.substring(secondIndex+2,thirdIndex);
  val.Smoke = rcvdMessage.substring(thirdIndex+2,fourthIndex);
  val.CO=rcvdMessage.substring(fourthIndex+2);
  return val;   
}

void sendMQTTMessage(char* Topic, char* Values) {
  if(!client.connected())
  {
    client.connect("ArduinoGW");
  }
  if(client.connected())
  {
    client.publish(Topic,Values);
  }
}

