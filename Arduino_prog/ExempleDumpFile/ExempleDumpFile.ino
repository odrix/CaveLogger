/*
  SD card file dump
 
 This example shows how to read a file from the SD card using the
 SD library and send it over the serial port.
 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created  22 December 2010
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */

#include <SD.h>

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 10;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

const int LED = 3;


void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);  
}

void loop()
{
  digitalWrite(LED, LOW);
  delay(100);
  
  // print the string when a newline arrives:
  if (stringComplete && inputString.indexOf("start") >= 0)  {
    digitalWrite(LED, HIGH);
    // clear the string:
    inputString = "";
    stringComplete = false;
    EnvoiDonnees();
  }
  
  // print the string when a newline arrives:
  if (stringComplete && inputString.indexOf("end") >= 0) {
    digitalWrite(LED, LOW);
    // clear the string:
    inputString = "";
    stringComplete = false;
    SupprimeDonnees();
  }
  digitalWrite(LED, HIGH);
  delay(100);
}

void EnvoiDonnees()
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("cavedata.log");

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close(); 
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  Serial.write("finish\n");
}

void SupprimeDonnees()
{
  SD.remove("cavedata.log");
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

