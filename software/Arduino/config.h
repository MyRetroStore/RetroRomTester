
#define VERSION "Version 1.0"

// Set to true or false
#define ReadOnly false //Set to true to only read ROM and don't compare to BIN file

//Set ROM Type
#define ROM_type 2316   // 2k
//#define ROM_type 2332 // 4k
//#define ROM_type 2364 // 8k

//Some Commodore computers have different revisions of 2332 / 2364 chips with alternate CE active high/low pins

//2332:
//CE1 Type 1  - active low 
//CE1 Type 2  - active high 
#define ROM2332_CE1_type 1

//2332:
//CE2 Type 1 - active low 
//CE2 Type 2 - active high 
#define ROM2332_CE2_type 2

//2364:
//Type 1  - CE1 active low 
//Type 2  - CE1 active high 
#define ROM2364_CE_type 1
