
#include <dht11.h>
#include <DS1302.h>
#include <SD.h>
#include <EEPROM.h>

// PIN et objet pour le capteur de temperature et humidité (DHT11)
#define DHT11PIN 2
dht11 DHT11;

// definition des PIN pour le module RTC et objet DS1302
const uint8_t CE_PIN   = 7;
const uint8_t IO_PIN   = 6;
const uint8_t SCLK_PIN = 5;
DS1302 rtc(CE_PIN, IO_PIN, SCLK_PIN);

/* Create buffers */
char bufTime[20];
String bufData = "";

const int chipSelect = 10;
const int INIT_BUTTON = 8;

// pour la capteur de luminosité
const int LIGHT_PIN = A0;
int minValueLight=1023;
int maxValueLight=0;

File myFile;

boolean log_ok=true;
const int adrMinValueLight=12;
const int adrMaxValueLight=16; // ATTENTION +4 car max 255 lors de l'ecriture en eeprom


/*------------------------------ READ DATA ----------------------------- */
void read_time()
{
  /* Get the current time and date from the chip */
  Time t = rtc.time();

  /* Format the time and date and insert into the temporary buffer */
  snprintf(bufTime, sizeof(bufTime), "%04d-%02d-%02d %02d:%02d:%02d",
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
}

void read_temperature_humidity()
{
  int chk = DHT11.read(DHT11PIN);

  if(chk==0)
  {
    bufData += "'h':" + String(DHT11.humidity, DEC) + ",'t':" + String(DHT11.temperature, DEC);
  }
  else
  {
    switch (chk)
    {
      case -1: Serial.print("Checksum error"); break;
      case -2: Serial.print("Time out error"); break;
      default: Serial.print("Unknown error"); break;
    }
  }  
}

void read_light()
{
   int value = analogRead(LIGHT_PIN);
   int pourcentageLight = map(value, minValueLight, maxValueLight, 5, 95);
   bufData += ",'l':" + String(pourcentageLight) + "}";
}



/*------------------------------ LOG DATA ----------------------------- */
void log_data_line()
{
  Serial.print("{'d':");
  Serial.print(bufTime);
  Serial.print(", ");
  Serial.println(bufData);
  log_ok=!log_ok;
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("cavedata.log", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print("{'d':");
    dataFile.print(bufTime);
    dataFile.print(", ");
    dataFile.println(bufData);
    dataFile.close();
    bufData = "";
    log_ok=true;
  }
  
  // une tentative suplementaire en reinitialisant la carte
  // dans le cas où elle à été enlevé
  if(!log_ok)
  {
    if (SD.begin(chipSelect)) 
    {
      log_data_line();
    }
  }
}

/*------------------------------ READ ALL DATA ----------------------------- */
void ReadAllDataToSerial()
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
}



/*------------------------------ INIT ----------------------------- */
void init_ligth()
{
  int sensorValue = 0;
  int isInit = LOW;
  minValueLight = EEPROM.read(adrMinValueLight);
  if(minValueLight == 0)
      minValueLight = 1023;
  maxValueLight = EEPROM.read(adrMaxValueLight) + EEPROM.read(adrMaxValueLight+1) + EEPROM.read(adrMaxValueLight+2) + EEPROM.read(adrMaxValueLight+3);
  Serial.println("avant : " + String(minValueLight) + " - " + String(maxValueLight));
  
  while(millis() < 5000)
  {
    isInit = digitalRead(INIT_BUTTON);
    sensorValue = analogRead(LIGHT_PIN);
    if(isInit == HIGH && sensorValue > maxValueLight) {
      maxValueLight = sensorValue;
    }
    
    if(isInit == HIGH && sensorValue < minValueLight) {
      minValueLight = sensorValue;
    }
  }
  Serial.println("apres : " + String(minValueLight) + " - " + String(maxValueLight));
  EEPROM.write(adrMinValueLight, minValueLight);
  eeprom_write_max(adrMaxValueLight, maxValueLight);
}

int eeprom_write_max(int adr, int value)
{
  if(value > 255)
  {
    EEPROM.write(adr, 255);
    eeprom_write_max(adr+1, value-255);
  }
  else
  {
    EEPROM.write(adr, value);
  }
}

void init_time()
{
  Serial.println("Init time......");
  boolean reinit=false;
  while(millis() < 8000)
  {
    if(digitalRead(INIT_BUTTON) == HIGH)
      reinit = true;
  }
  
  if(reinit)
  {
    /* Make a new time object to set the date and time */
    Time t(2013, 9, 04, 00, 45, 00, 3);
  
    /* Set the time and date on the chip */
    rtc.time(t);
  }
}

void setup()
{
  Serial.begin(9600);
  /* Initialize a new chip by turning off write protection and clearing the
     clock halt flag. These methods needn't always be called. See the DS1302
     datasheet for details. */
  rtc.write_protect(false);
  rtc.halt(false);
 
   // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  
  init_ligth();
  init_time();
}


/*------------------------------ LOOP ----------------------------- */
void loop()
{
  read_time();
  read_temperature_humidity();
  read_light();
  log_data_line();
  
  delay(100);
}
