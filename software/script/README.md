# Creating MD5 hashes

The python script was developed in Linux, but should run on any OS with python 3.x

## Introduction

The python script **create_md5.py** is used to generate MD5 hashes from ROM files. This is useful if the ROM you are testing does not have a MD5 hash pre-configured in the code. 

As the Arduino Mega 2560 has a slimited amount of RAM, when reading the ROM the contents are reach in 4k chunks. For this reason on 8k ROM's there are 2 sets of MD5 hashes generated. 
 
 The hashes are stored in the file [crc.h](https://github.com/MyRetroStore/RetroRomTester/blob/main/software/Arduino/crc.h) and take the format of the following array.
 8k ROM:
 ``` 
 {"basic_901226_01", "ef209f5b3d59a878a9be73eb254aa429", "bc16916b5f8f5eabf25cfadb97cf0499"},
```
 2k / 4k ROM:
 ```
  {"basic_4_d000_901465_21", "ab780e94772dca756a0678a17b5bc3a2"},
 ```
 
## Usage

First you need to copy ```crc.h``` from the Arduino folder to the same folder as ```create_md5.py``` as this script updates the file with the new hash value.
Then run:
```
./create_md5.py -i filename
```
This calculates the md5 hash for the ROM binary (filename) and updates the crc.h file. 

 Example:
 ```
 ./create_md5.py -i kernal.901227-01.bin
 ```
Once completed copy ```crc.h``` back to the Arduino folder and compile and upload the sketch to the Mega2560.
 
 ## To Do
 - While basic error checking is done in the python script, there is a lot more error checking that cold be done so not all errors will be catered for (Eg if you had to remove the marker text from crc.h then the script will probably throw a weird error)
 - I'm not a programmer so while this script seems to work, it's not the most efficient and there are probably a lot better ways to do things. 
 


## License
CC BY-NC-SA 4.0
