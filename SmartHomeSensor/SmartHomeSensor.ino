#include <dht11.h>

#define DHT11PIN 2
#define MQ2PIN A0
#define MQ7PIN A1

dht11 DHT11;
float Temp;
float Humidity;
boolean MQ2Alert = false;
boolean MQ7Alert = false;

//MQ2 threshold for alarm
int MQ2Thres = 150;
//MQ7 threshold for alarm
int MQ7Thres = 150;

void setup()
{
  Serial.begin(9600);
  //Serial.print("DHT11 LIBRARY VERSION: ");
  //Serial.println(DHT11LIB_VERSION);
  //Serial.println();
  pinMode(MQ2PIN, INPUT);
  pinMode(MQ7PIN, INPUT);
  delay(3000);
}

void loop()
{
  Humidity = DHTReadHumidity();
  Temp = DHTReadTemp();
  MQ2Alert = MQ2SmokeDetected();
  MQ7Alert = MQ7CODetected();
  delay(3000);
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

  return((float)DHT11.temperature);
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
  return((float)DHT11.humidity);
}

boolean MQ2SmokeDetected()
{
  int analogSensor = analogRead(MQ2PIN);
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > MQ2Thres)
  {
    Serial.println("smoke detected");
    Serial.println(analogSensor);
    return true;
  }
  else
  {
    Serial.println("No smoke detected");
    Serial.println(analogSensor);
    return false;
  }
}

boolean MQ7CODetected()
{
  int analogSensor = analogRead(MQ7PIN);
  Serial.print("Pin A1: ");
  Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > MQ7Thres)
  {
    Serial.println("CO detected");
    Serial.println(analogSensor);
    return true;
  }
  else
  {
    Serial.println("No CO detected");
    Serial.println(analogSensor);
    return false;
  }
}


