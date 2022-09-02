#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <HTTPClient.h>
#include <WiFi.h>

#define LED_BUILTIN 2
#define RXD2 16
#define TXD2 17
#define GPSBaud 9600

TinyGPSPlus gps;
SoftwareSerial ss(RXD2, TXD2);

const String IP = "192.168.0.101";
const String ID = "1A2009";

float latitud, longitud;
int year,sat;
byte month, day, hour, minute, second, hundredths;
int incomingByte = 0; // for incoming serial data

float readTemp()
{ 
  int analogValue = analogRead(A0);
  float celsius =  analogValue/(5);
  return celsius;
}

String getDate(String url)
{
  HTTPClient http;
  
    // Your Domain name with URL path or IP address with path
  
    http.begin(url);
     
    // Send HTTP GET request
    int httpResponseCode = http.GET();
  
    if (httpResponseCode>0) 
    {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
          digitalWrite(LED_BUILTIN, LOW);
          delay(200);
          digitalWrite(LED_BUILTIN, HIGH);
          delay(200);
          return payload;
    }
    else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
          return "";
    }
    // Free resources
    http.end();
}

void setup() 
{
  char* SSDI="tony";
  char* PASS="79946320";

  pinMode(A0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  ss.begin(GPSBaud);
  
  Serial.print("Connecting to WiFi\n");
  WiFi.begin(SSDI, PASS, 6);
  while (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  Serial.println(" Connected!");
  
}

void loop() 
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second

  String URL;

  bool newData = false;
  unsigned long chars;
  
  /*// send data only when you receive data:
  if (Serial2.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial2.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }*/

  for (unsigned long start = millis(); millis() - start < 800;)
  {
    while (ss.available() > 0)
    {
      if (gps.encode(ss.read())) newData = true;
    }
  }

  if (newData)
  {
//latitud y longitud, numero de satelites disponibles    
    if (gps.location.isValid()) 
    {
      Serial.print("LAT=");
      Serial.print(gps.location.lat(), 6);
      Serial.print(" LON=");
      Serial.print(gps.location.lng(), 6);
      Serial.print(" SAT=");
      Serial.println(gps.satellites.value());
    }
    else 
    {
      Serial.print(F("INVALID"));
    }
//Fecha y hora 
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();


  URL= "http://"+ IP + "/getDate"  ;
  String Date = getDate(URL);
  
  Serial.println("-->Fecha: "+Date);
  
  URL = "http://"+ IP + "/send-data/"+ ID +"||";
  URL.concat(readTemp());//Concatenamos la Temperatura
  URL = URL + "||" + String(gps.location.lat(),6) + "," + String(gps.location.lng(),6) ;
  URL = URL + "||connected||" + Date;
  
  getDate(URL);
  
  delay(10000); // TODO: Build something amazing!
  }
}
