#include <JeeLib.h>
#include <dht11.h>
#include <RH_ASK.h>
#include <SPI.h>


#define DHT11PIN 2
#define MQ2PIN A0
#define MQ7PIN A1

dht11 DHT11;

float Temp;
float Humidity;
boolean MQ2Alert = false;
boolean MQ7Alert = false;
char str_hum[5];
char str_temp[5];

//Update the below value to define where the sensor will be located - will be used in mqtt topic on gateway code
char str_location[10] = "zolder";

//MQ2 threshold for alarm
int MQ2Thres = 90;
//MQ7 threshold for alarm
int MQ7Thres = 90;

//Watchdog for low power consumption
ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup the watchdog

//To change the standard constructor of the RH_ASK driver
// Paramters are as follow:
// \param[in] speed The desired bit rate in bits per second
// \param[in] rxPin The pin that is used to get data from the receiver
// \param[in] txPin The pin that is used to send data to the transmitter
// \param[in] pttPin The pin that is connected to the transmitter controller. It will be set HIGH to enable the transmitter (unless pttInverted is true).
// \param[in] pttInverted true if you desire the pttin to be inverted so that LOW wil enable the transmitter.
// RH_ASK(uint16_t speed = 2000, uint8_t rxPin = 11, uint8_t txPin = 12, uint8_t pttPin = 10, bool pttInverted = false)
RH_ASK driver(2000, 3, 4, 5);

void setup()
{
  Serial.begin(9600);
  pinMode(MQ2PIN, INPUT);
  pinMode(MQ7PIN, INPUT);
  if (!driver.init())
         Serial.println("init failed");
  delay(3000);
}

void loop()
{
  delay(1000);
  Humidity = DHTReadHumidity();
  dtostrf(Humidity, 3, 1, str_hum);
  Temp = DHTReadTemp();
  dtostrf(Temp, 3, 1, str_temp);
  MQ2Alert = MQ2SmokeDetected();
  MQ7Alert = MQ7CODetected();
  char sendMsg[20];
  //Message structure: locationNode&&Humidity&&Temperature&&MQ2Alert&&MQ7Alert 
  sprintf(sendMsg,"%s&&%s&&%s&&%d&&%d",str_location, str_temp, str_hum, MQ2Alert, MQ7Alert);   
  
  sendMessage(sendMsg);
  Sleepy::loseSomeTime(60000);
}

float sendMessage(const char message[])
{
  const char *msg = message;
  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
  delay(200);
}

float DHTReadTemp()
{
  int chk = DHT11.read(DHT11PIN);

  switch (chk)
  {
    case DHTLIB_OK:
      //Serial.println("OK");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      //Serial.println("Checksum error");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      //Serial.println("Time out error");
      break;
    default:
      //Serial.println("Unknown error");
      break;
  }

  return ((float)DHT11.temperature);
}

float DHTReadHumidity()
{
  int chk = DHT11.read(DHT11PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      //Serial.println("OK");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      //Serial.println("Checksum error");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      //Serial.println("Time out error");
      break;
    default:
      //Serial.println("Unknown error");
      break;
  }
  //Serial.println((float)DHT11.humidity,2);
  return ((float)DHT11.humidity);
}

//Sensitive for Methane, Butane, LPG, smoke.
boolean MQ2SmokeDetected()
{
  int analogSensor = analogRead(MQ2PIN);
  //Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > MQ2Thres)
  {
    //Serial.println("smoke detected");
    //Serial.println(analogSensor);
    return true;
  }
  else
  {
    //Serial.println("No smoke detected");
    //Serial.println(analogSensor);
    return false;
  }
}

//Sensitive for Carbon Monoxide
boolean MQ7CODetected()
{
  int analogSensor = analogRead(MQ7PIN);
  //Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > MQ7Thres)
  {
    //Serial.println("CO detected");
    //Serial.println(analogSensor);
    return true;
  }
  else
  {
    //Serial.println("No CO detected");
    //Serial.println(analogSensor);
    return false;
  }
}


