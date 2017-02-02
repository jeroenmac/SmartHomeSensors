
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <Ethernet.h>
#include <PubSubClient.h>

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
    Serial.println("in loop");
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
        int i;
        for (i=0;i<buflen;i++)               
        {
        askMessage += char(buf[i]);  
        } 
    char Topic[10];
    char Values[10];
    getTopic(askMessage).toCharArray(Topic, 10);
    getValues(askMessage).toCharArray(Values,10);

    sendMQTTMessage(Topic, Values);
    }
}

//Will reutn the topic from the received message. 
//Can be used to submit via MQTT as the topic
String getTopic(String rcvdMessage)
{
    String Topic; 
    uint8_t indexCutoff;
    indexCutoff = rcvdMessage.indexOf("&&");
    Topic = rcvdMessage.substring(0,indexCutoff); 
    return Topic; 
}

//will return the value of the received message
//can be used to submit via MQTT as value
String getValues(String rcvdMessage)
{
    String Values; 
    uint8_t indexCutoff;
    indexCutoff = rcvdMessage.indexOf("&&");
    Values = rcvdMessage.substring(indexCutoff); 
    return Values; 
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

