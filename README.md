# Retro ROM Tester
Arduino shield to read and verify 2316, 2332 and 2364 ROM's from retro computers like the Commodore 64, VIC-20 and PET.

## Introduction
Moderm EPROM programmers like the TL866 are unable to read obsolete 23XX ROM chips used in many retro Computers like the Commodore 64, VIC-20 and PET.

I built a shield for the Arduino Mega 2560 which reads the ROM and compares the MD5 hash value to know good ROMs found on [Zimmers.net](https://zimmers.net/anonftp/pub/cbm/firmware/index.html) with a option of displaying the contents of the ROM via the Arduino IDE serial monitor. 
![Shield](https://github.com/MyRetroStore/RetroRomTester/blob/main/hardware/Retro%20ROM%20Tester%20Shield.jpg?raw=true)


## ROMs Supported
- 2316 (2k x 8)
- 2332 (4k x 8) - both active high/low CE1/CE2 versions
- 2364 (8k x 8) - both active high/low CE versions

## Usage

This Arduino sketch was built and tested using Arduino IDE 1.8.19 but should work with later versions.

Before uploading the .ino to the Mega2560, review the config.h file and change the paramaters as required:

- Set ROM Type (2316 / 2332 / 2364)
- Set CE pins (active low or high) - Commodore MOS released multiple versions of the 2332 and 2364 EPROMs with different active low/high CE pins.
- To display ROM contents set ReadOnly to true

Once the code has been uploaded insert the chip (Remove power before inserting the ROM) and open the serial monitor set to 115200 baud.

Program will run and compare the md5 hash of the ROM under test, or display the contents of the ROM.

![Serial Output](https://github.com/MyRetroStore/RetroRomTester/blob/main/software/serial-output.png?raw=true)

The Arduino code comes pre-configured with a set of md5 hashes from most of the Commodore ROMs, but additional hashes can be added using the supplied python [script](https://github.com/MyRetroStore/RetroRomTester/tree/main/software)


##  Shield

The gerber files for the shield can be found under hardware/gerbers

![Retro ROM Tester](https://github.com/MyRetroStore/RetroRomTester/blob/main/hardware/Retro%20ROM%20Tester.png?raw=true)

## BOM
| Reference | Value |
| ------ | ------ |
| C1 | 100nF Ceramic Capacitor 0805 |
| D1 | Green LED 0805 |
| D2 | Red LED 0805 |
| F1 | 100mA poly fuse 0805 |
| RN1, RN2 | 10k Resistor Array 9 pin |
| R1, R2 | 1k Resistor 0805 |
| U1 | 24 Pin ZIF Socket |
| SW1 | Push Button Switch 6x6x6 |
| | 2.54mm SIL Male Headers

## Purchasing

If you don't want build the shield yourself, assembled shields can be purchased from:
- [MyRetroStore](https://myretrostore.co.uk/)
- [eBay](https://www.ebay.co.uk/str/myretrostoreuk)

## License
CC BY-NC-SA 4.0


[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/H2H8RDX9W)
