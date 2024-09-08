/*
  myretrostore.co.uk
  https://github.com/MyRetroStore/RetroRomTester

  This code is for the Retro ROM Tester Shield for the Arduino Mega2560 which is used to read and validate 2316/2332/2364 ROM's from retro computers
  like the Commodore 64, VIC-20, PET.

  Data is read from the ROM and compared against known binary ROM md5 hash dumps.

  Additional md5 hashes can be generated and import using the create_md5.py python script

  Tested using Arduino v1.8.19

  Set READONLY flag in config.h to only read contents of ROM and output to serial port at 115200

  See config.h for setting ROM type and other options

*/

#include "config.h"
#include "MD5.h"
#include "crc.h"

// Setup definitions for 2316
const  uint8_t DATABUS[] = {43, 44, 45, 35, 34, 33, 32, 31};
const  uint8_t ROM2316_ADDRBUS[] = {42, 41, 40, 39, 38, 37, 36, A2, 24, 28, 29 };
#define ROM2316_CE1 12
#define ROM2316_CE2 30
#define ROM2316_CE3 13

// Setup definitions for 2332
const  uint8_t ROM2332_ADDRBUS[] = {42, 41, 40, 39, 38, 37, 36, A2, 24, 28, 29, 30 };
#define ROM2332_CE1 12
#define ROM2332_CE2 13

// Setup definitions for 2364
const  uint8_t ROM2364_ADDRBUS[] = {42, 41, 40, 39, 38, 37, 36, A2, 24, 28, 29, 30, 13 };
#define ROM2364_CE 12

#define LED_Pass A1
#define LED_Fail A0

uint16_t BIN_SIZE;

uint16_t ROM_counter = 0;
uint16_t counter;
boolean hashmatch = false;
size_t index = 0;

char ROM[4100] = {};  // Buffer to hold 4k of ROM
int ledState = LOW;     // the current state of LED

//=======================================================================================
// Initialize pins on 2332
//=======================================================================================
void Initialize_2332() {

  pinMode(ROM2332_CE1, OUTPUT);
  digitalWrite(ROM2332_CE1, LOW);
  pinMode(ROM2332_CE2, OUTPUT);

  if (ROM2332_CE1_type == 1)
  {
    digitalWrite(ROM2332_CE2, LOW); // Active LOW
  } else {
    digitalWrite(ROM2332_CE2, HIGH);//Active HIGH
  }

  if (ROM2332_CE2_type == 1)
  {
    digitalWrite(ROM2332_CE2, HIGH); // Active LOW, so set HIGH to disable
  } else {
    digitalWrite(ROM2332_CE2, LOW);//Active HIGH, so set LOW to disable
  }

  for (int i = 0; i < sizeof(ROM2332_ADDRBUS); i++) pinMode(ROM2332_ADDRBUS[i], OUTPUT);

}

//=======================================================================================
// Initialize pins on 2364
//=======================================================================================
void Initialize_2364() {

  pinMode(ROM2364_CE, OUTPUT);

  if (ROM2364_CE_type == 1)  //
  {
    digitalWrite(ROM2364_CE, HIGH); //Active LOW, so set HIGH to disableCheck if CE active low or high
  } else {
    digitalWrite(ROM2364_CE, LOW);//Active HIGH, so set HIGH to disableCheck if CE active low or high
  }

  for (int i = 0; i < sizeof(ROM2364_ADDRBUS); i++) pinMode(ROM2364_ADDRBUS[i], OUTPUT);
  for (int i = 0; i < sizeof(DATABUS); i++) pinMode(DATABUS[i], INPUT);

}

//=======================================================================================
// Initialize pins on 2316
//=======================================================================================
void Initialize_2316() {
  for (int i = 0; i < sizeof(ROM2316_ADDRBUS); i++) pinMode(ROM2316_ADDRBUS[i], OUTPUT);
  pinMode(ROM2316_CE1, OUTPUT);
  digitalWrite(ROM2316_CE1, HIGH);  // Active LOW, Set HIGH to disable
  pinMode(ROM2316_CE2, OUTPUT);
  digitalWrite(ROM2316_CE2, LOW);   // Active LOW
  pinMode(ROM2316_CE3, OUTPUT);
  digitalWrite(ROM2316_CE3, HIGH);   // Active HIGH
}


//=======================================================================================
// Pass / Fail LEDs and HALT
//=======================================================================================
void Display_LEDs(boolean error) {

  if (error == true) {  // Fail
    digitalWrite(LED_Pass, LOW);
    while (1) {
      digitalWrite(LED_Fail, LOW);
      delay(500);
      digitalWrite(LED_Fail, HIGH);
      delay(500);
    }

  } else {  // Pass
    digitalWrite(LED_Pass, HIGH);
    digitalWrite(LED_Fail, LOW);
    while (1);
  }
}

//=======================================================================================
// Check config file is setup
//=======================================================================================
void check_config() {

#ifndef ROM_type  //Check if ROM type has been set
  Serial.println("ROM type not defined. See config.h");
  Display_LEDs(true);
#endif

  switch (ROM_type) {   //Set ROM size based on chip type
    case 2316:
      BIN_SIZE = 2048;
      break;
    case 2332:
      BIN_SIZE = 4096;
      break;
    case 2364:
      BIN_SIZE = 8192;
      break;
    default:
      Serial.println("Error. ROM Type not recognized");
      Display_LEDs(true);
  }


  if (ROM_type == 2332) {
#ifndef ROM2332_CE1_type //Check if CE1 & CE2 types defined
    Serial.println("ROM type not defined. See config.h");
    Display_LEDs(true);
#endif

#ifndef ROM2332_CE2_type
    Serial.println("ROM type not defined. See config.h");
    Display_LEDs(true);
#endif
  }

  if (ROM_type == 2364) {

#ifndef ROM2364_CE_type //Check if CE type defined
    Serial.println("ROM CE type not defined. See config.h");
    Display_LEDs(true);
#endif
  }

}

//=======================================================================================
// Setup
//=======================================================================================
void setup() {
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB port only

  Serial.println("\n======================");
  Serial.println("   Retro ROM Tester");
  Serial.println("  myretrostore.co.uk");
  Serial.print("     "); Serial.println(VERSION);
  Serial.println("======================");

  pinMode(LED_Fail, OUTPUT);
  pinMode(LED_Pass, OUTPUT);

  digitalWrite(LED_Fail, LOW);
  digitalWrite(LED_Pass, LOW);

  check_config(); // Check options are set in config.h

}

//=======================================================================================
// Set Address for 2332
//=======================================================================================
void setAddress_2332(uint16_t addr) {
  for (uint8_t bitno = 0 ; bitno < sizeof(ROM2332_ADDRBUS) ; bitno++) {
    if (bitRead(addr, bitno) == 1) {
      digitalWrite(ROM2332_ADDRBUS[bitno], HIGH);
    } else {
      digitalWrite(ROM2332_ADDRBUS[bitno], LOW);
    }
  }
  return;
}

//=======================================================================================
// Set Address for 2316
//=======================================================================================
void setAddress_2316(uint16_t addr) {
  for (uint8_t bitno = 0 ; bitno < sizeof(ROM2316_ADDRBUS) ; bitno++) {
    if (bitRead(addr, bitno) == 1) {
      digitalWrite(ROM2316_ADDRBUS[bitno], HIGH);
    } else {
      digitalWrite(ROM2316_ADDRBUS[bitno], LOW);
    }
  }
  return;
}

//=======================================================================================
// Set Address for 2364
//=======================================================================================
void setAddress_2364(uint16_t addr) {
  for (uint8_t bitno = 0 ; bitno < sizeof(ROM2364_ADDRBUS) ; bitno++) {
    if (bitRead(addr, bitno) == 1) {
      digitalWrite(ROM2364_ADDRBUS[bitno], HIGH);
    } else {
      digitalWrite(ROM2364_ADDRBUS[bitno], LOW);
    }
  }
  return;
}

//=======================================================================================
// Read Address for 2316
//=======================================================================================
byte readAddress_2316(int addr) {
  digitalWrite(ROM2316_CE1, HIGH);
  byte data = 0;
  setAddress_2316(addr);
  digitalWrite(ROM2316_CE1, LOW);
  delayMicroseconds(1);
  for (int bitno = 0; bitno < 8; bitno++) {
    if (digitalRead(DATABUS[bitno]) == HIGH) {
      bitSet(data, bitno);
    }
  }
  digitalWrite(ROM2316_CE1, HIGH);
  return data;
}

//=======================================================================================
// Read Address for 2332
//=======================================================================================
byte readAddress_2332(int addr) {
  if (ROM2332_CE2_type == 1)
  {
    digitalWrite(ROM2332_CE2, HIGH); //Active LOW, so set HIGH to disable
  } else {
    digitalWrite(ROM2332_CE2, LOW);//Active LOW, so set HIGH to disable
  }

  byte data = 0;
  setAddress_2332(addr);

  if (ROM2332_CE2_type == 1)
  {
    digitalWrite(ROM2332_CE2, LOW);
  } else {
    digitalWrite(ROM2332_CE2, HIGH);
  }
  delayMicroseconds(1);

  for (int bitno = 0; bitno < 8; bitno++) {
    if (digitalRead(DATABUS[bitno]) == HIGH) {
      bitSet(data, bitno);
    }
  }
  if (ROM2332_CE2_type == 1)
  {
    digitalWrite(ROM2332_CE2, HIGH); //Active LOW, so set HIGH to disableCheck if CE active low or high
  } else {
    digitalWrite(ROM2332_CE2, LOW);//Active LOW, so set HIGH to disableCheck if CE active low or high
  }
  return data;
}

//=======================================================================================
// Read Address for 2364
//=======================================================================================
byte readAddress_2364(int addr) {

  if (ROM2364_CE_type == 1)  //
  {
    digitalWrite(ROM2364_CE, HIGH); //Active LOW, so set HIGH to disable
  } else {
    digitalWrite(ROM2364_CE, LOW);//Active LOW, so set HIGH to disable
  }

  byte data = 0;
  setAddress_2364(addr);

  if (ROM2364_CE_type == 1)
  {
    digitalWrite(ROM2364_CE, LOW);
  } else {
    digitalWrite(ROM2364_CE, HIGH);
  }

  delayMicroseconds(1); // Wait 1micro sec
  for (int bitno = 0; bitno < 8; bitno++) {
    if (digitalRead(DATABUS[bitno]) == HIGH) {
      bitSet(data, bitno);
    }
  }
  if (ROM2364_CE_type == 1)
  {
    digitalWrite(ROM2364_CE, HIGH); //Active LOW, so set HIGH to disableCheck if CE active low or high
  } else {
    digitalWrite(ROM2364_CE, LOW);//Active LOW, so set HIGH to disableCheck if CE active low or high
  }
  return data;
}

//=======================================================================================
// Main Loop
//=======================================================================================

void loop() {

  //Calculate size of md5 arrays
  const size_t md5Cnt_8192 = sizeof md5_8192 / sizeof * md5_8192;
  const size_t md5Cnt_4096 = sizeof md5_4096 / sizeof * md5_4096;
  uint8_t ROM_Byte;
  char str[40];

  if (ReadOnly == false) {
    Serial.println("Reading EPROM...");
  } else {
    Serial.println("Dumping EPROM (Not comparing)\n");
  }

  //Read entire rom and calculate md5
  for (int i = 0; i < sizeof(DATABUS); i++) pinMode(DATABUS[i], INPUT);

  if (BIN_SIZE == 2048) {   //2316 Chip
    Initialize_2316();
  }

  if (BIN_SIZE == 4096) {   //2332 Chip
    Initialize_2332();
  }

  if (BIN_SIZE == 8192) {   //2364 Chip
    Initialize_2364();
  }

  for (uint16_t address = 0; address < BIN_SIZE; address++)
  {
    if (ROM_type == 2316) {
      delay(1);
      ROM_Byte = readAddress_2316(address);
    }

    if (ROM_type == 2332) {
      delay(1);
      ROM_Byte = readAddress_2332(address);
    }

    if (ROM_type == 2364) {
      delay(1);
      ROM_Byte = readAddress_2364(address);
    }

    ROM[ROM_counter] = ROM_Byte;
    ROM_counter++;

    unsigned char* hash;
    char *md5str;

    if ( (address == 4095 ) or ((address == 2047) and (ROM_type == 2316)))  { //Stop at 4096 as Arduino only has 8k of RAM so we do this in 2 x 4k chunks.
      //Or stop at 2048 if it's a 2316 ROM

      ROM_counter = 0;

      if (ROM_type == 2316) { // If it's a 2k ROM then calculate hash on 2k size, otherwise it's 4k (hash 1 & 2)
        hash = MD5::make_hash(ROM, 2048);
        md5str = MD5::make_digest(hash, 16);
      }

      if (ROM_type == 2316) {
        hash = MD5::make_hash(ROM, 2048);
        md5str = MD5::make_digest(hash, 16);
      } else if (ROM_type == 2332) {
        hash = MD5::make_hash(ROM);
        md5str = MD5::make_digest(hash, 16);
      }

      if ((ROM_type == 2316) or (ROM_type == 2332)) {
        // 2k or 4k ROM, so only one hash value
        for (index; index < md5Cnt_4096; index++) {
          if (strcmp_P(md5str, md5_4096[index].hash1) == 0) {
            //Match on first hash
            Serial.println("\nFound a valid MD5 hash");
            Serial.print("ROM is "); Serial.print((__FlashStringHelper*) md5_4096[index].fileName);
            Display_LEDs(false);
          }
        }

      }

      if (ROM_type == 2364) {
        //8k ROM, so 2 hash values
        hash = MD5::make_hash(ROM);
        md5str = MD5::make_digest(hash, 16);

        for (index; index < md5Cnt_8192; index++) {
          if (strcmp_P(md5str, md5_8192[index].hash1) == 0) {
            //Match on first hash
            hashmatch = true;
            break;
          }
        }
      }

      if ((ReadOnly == false) and (hashmatch == false)) { // Failed to match first hash.
        Serial.println("\nUnable to find valid MD5 match for ROM");
        Serial.print("\nMD5 read from ROM is "); Serial.println(md5str);

        Serial.println("\nCheck that the ROM type is included in crc.h otherwise the ROM is corrupt");
        Serial.println("Try the ReadOnly switch in the config.h and examine the dump");
        Display_LEDs(true);
      }
    }

    if (address == 8191 ) { // Process next 4k chunk

      hashmatch = false;
      ROM_counter = 0;
      unsigned char* hash = MD5::make_hash(ROM);
      char *md5str = MD5::make_digest(hash, 16);

      // Time to match the second hash
      for (index; index < md5Cnt_8192; index++) {
        if (strcmp_P(md5str, md5_8192[index].hash2) == 0) {
          Serial.println("\nFound a valid MD5 hash");
          Serial.print("ROM is "); Serial.print((__FlashStringHelper*) md5_8192[index].fileName);
          Display_LEDs(false);
        } else {
          Serial.println("\nUnable to find valid MD5 match for ROM");
          Serial.print("\nMD5 read from ROM is "); Serial.println(md5str);
        }
      }
    }

    if (address % 128 == 0) //Visual progress feedback. Toggle LEDs
    {
      digitalWrite(LED_Pass, ledState);
      digitalWrite(LED_Fail, !ledState);
      ledState = !ledState;
    }

    if (ReadOnly == true) // If ReadOnly flag set we are only going to dump ROM to screen.
    {
      if (address % 16 == 0) // Setup formatting
      {
        sprintf(str, "\r\n%04X:", address);
        Serial.print(str);
      }
      sprintf(str, " %02X", ROM_Byte);
      Serial.print(str);
    }
    delayMicroseconds(1);
  }

  if (ReadOnly == true) {
    Serial.println("\n\nEPROM Dump Complete");
  }

  Display_LEDs(true);
}
