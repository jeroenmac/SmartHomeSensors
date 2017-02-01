
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

// Constructor.
// \param[in] speed The desired bit rate in bits per second
// \param[in] rxPin The pin that is used to get data from the receiver
// \param[in] txPin The pin that is used to send data to the transmitter
// \param[in] pttPin The pin that is connected to the transmitter controller. It will be set HIGH to enable the transmitter (unless pttInverted is true).
// \param[in] pttInverted true if you desire the pttin to be inverted so that LOW wil enable the transmitter.
// RH_ASK(uint16_t speed = 2000, uint8_t rxPin = 11, uint8_t txPin = 12, uint8_t pttPin = 10, bool pttInverted =

RH_ASK driver(2000, 2,3,4); // ESP8266: do not use pin 11

void setup()
{
    Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");
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
        for (i=0;i<buflen;i++)               // Get the two first bytes
        {
        askMessage += char(buf[i]);  
        } 
        Serial.println(askMessage);
    }
}
