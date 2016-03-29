
#include <SoftwareSerial.h>
#include "Time.h"
#include "Timer.h"


//wAV Trigger configurations
#define AC3  1  //3 aircraft
#define SFX  2  //all sound effects

//  ******  Digital OUTPUTS    ******

//#define avaialbe IO Pin  0  reserved to use with joystick center press select SW

#define LED3_3  1
#define LED4_3  2
#define LED1_3  3
#define LED2_3  4

#define LED3_2  5
#define LED4_2  6
#define LED1_2  7
#define LED2_2  8

#define LED3_1  9
#define LED4_1  10
#define LED2_1  11
#define LED1_1  12

#define LED3_4  13

// Serial 3   14 & 15  WAV Trigger Comms
// Serial 2   16 & 17  LCD Comms

#define LED4_4  18
#define LED1_4  19
#define LED2_4  20
//*************************

//*************************
#define BLUE3   21
#define GREEN3  22
#define RED3    23

// Using 24-39 as Inputs

#define BLUE4   40
#define GREEN4  41
#define RED4    42

#define BLUE2   43
#define GREEN2  44
#define RED2    45

#define BLUE1   46
#define GREEN1  47
#define RED1    48

// Using pins 49 & 50 for cfg and stop all wavs
// Using pins 51,52 & 53 for reading foot SWs



//*************************


//  ******  Digital INPUTS    ******
#define BUTTON3_3  24
#define BUTTON4_3  25
#define BUTTON1_3  26
#define BUTTON2_3  27

#define BUTTON3_2  28
#define BUTTON4_2  29
#define BUTTON1_2  30
#define BUTTON2_2  31

#define BUTTON3_1  32
#define BUTTON4_1  33
#define BUTTON1_1  34
#define BUTTON2_1  35

#define BUTTON3_4  36
#define BUTTON4_4  37
#define BUTTON1_4  38
#define BUTTON2_4  39

#define Change_CFG_BUTTON   49
#define STOP_ALL_BUTTON     50

#define FOOT_SW1  51
#define FOOT_SW2  52
#define FOOT_SW3  53

//  ****** All other preprocessor definitions  ******
#define MAX_BUTTONS 16

#define RED  99
#define BLUE 100
#define GREEN 101
#define YELLOW 102
#define ALL_COLORS -9

//WAV Triggger defs
#define SINGLE 0x00
#define POLY   0x01
#define MAX_WAVS_PLAYING 20

//Globals
byte      msg_wt001[] = {0xF0, 0xAA, 0x08, 0x03, 0x01, 0x00, 0x00, 0x55};
byte      msg_wt010[] = {0xF0, 0xAA, 0x08, 0x03, 0x01, 0x0A, 0x00, 0x55};
byte  msg_wt_single[] = {0xf0, 0xaa, 0x08, 0x03, 0x00, 0x0a, 0x00, 0x55};  //example is for track 11 0x0A
byte    msg_wt_poly[] = {0xf0, 0xaa, 0x08, 0x03, 0x01, 0x0a, 0x00, 0x55};  //example is for track 11 0x0A
byte   msg_wt_pause[] = {0xf0, 0xaa, 0x08, 0x03, 0x02, 0x0a, 0x00, 0x55};  //example is for track 11 0x0A
byte  msg_wt_resume[] = {0xf0, 0xaa, 0x08, 0x03, 0x03, 0x0a, 0x00, 0x55};  //example is for track 11 0x0A
byte    msg_wt_stop[] = {0xf0, 0xaa, 0x08, 0x03, 0x04, 0x0a, 0x00, 0x55};  //example is for track 11 0x0A
byte msg_wt_stopall[] = {0xf0, 0xaa, 0x05, 0x04, 0x55};
byte     msg_wt_req[] = {0xf0, 0xaa, 0x05, 0x01, 0x55};
byte msg_wt_get_status[] = {0xf0, 0xaa, 0x05, 0x07, 0x55};
byte   msg_wt_trackvol[] = {0xf0, 0xaa, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x55};
byte  msg_wt_enableamp[] = {0xf0, 0xaa, 0x06, 0x09, 0x01, 0x55};
byte msg_wt_disableamp[] = {0xf0, 0xaa, 0x06, 0x09, 0x00, 0x55};


int tracks_playing[MAX_WAVS_PLAYING] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //Example:  Tracks 5 & 10 is playing  {5,10,0,0,0,0,0....} sequentually populated...could have track #5 playing poly 2x {5,5,10,0,0,0....}
int LEDenable_Button[MAX_BUTTONS] = {LED1_1,LED2_1,LED3_1,LED4_1,LED1_2,LED2_2,LED3_2,LED4_2,LED1_3,LED2_3,LED3_3,LED4_3,LED1_4,LED2_4,LED3_4,LED4_4};  //Defines which physical buttons have LEDs enabled
int Button[MAX_BUTTONS] = {BUTTON1_1,BUTTON2_1,BUTTON3_1,BUTTON4_1,BUTTON1_2,BUTTON2_2,BUTTON3_2,BUTTON4_2,BUTTON1_3,BUTTON2_3,BUTTON3_3,BUTTON4_3,BUTTON1_4,BUTTON2_4,BUTTON3_4,BUTTON4_4};  //Defines physical order of buttons
int Chase_Seq[MAX_BUTTONS] = {LED1_1,LED2_1,LED1_2,LED2_2,LED1_3,LED2_3,LED3_1,LED4_1,LED3_2,LED4_2,LED3_3,LED4_3,LED1_4,LED2_4,LED3_4,LED4_4};
int ConfigurationNumber = 0;

Timer t;
time_t trackStart_secs;

int CurrentPlayingTrackLength = 1000;
String CurrentPlayingTrackTitle = " ";
bool AmpEnabled = 0;
int FootSWSongNumber = 0;

typedef struct  {
  int Button_DI_Num;
  String LCD_Msg;
  int LED_DO_Num;
  int Color;
  int TrackNumber;
  bool SinglePoly;
  int Volume;  //in dB
  int Track_Duration_Secs;
} BUTTON;

#define NUM_CONFIGS 7

BUTTON KeyConfig[MAX_BUTTONS*NUM_CONFIGS] = {
  //********************  Config #0:  F16  ********************************//
  {BUTTON1_1,"F16 Top Gun Song",LED1_1,BLUE,1,SINGLE,0,301},     //Button 1
  {BUTTON2_1,"Gold Finger 007 ",LED2_1,BLUE,42,SINGLE,0,128},    //Button 2
  {BUTTON3_1,"USAF Song       ",LED3_1,BLUE,61,SINGLE,3,165},      //Button 3
  {BUTTON4_1,"Mission Impossbl",LED4_1,BLUE,71,SINGLE,0,196},   //Button 4

  {BUTTON1_2,"HD Machine Gun!!",LED1_2,RED,8,POLY,0,0},   //Button 5
  {BUTTON2_2,"Gun Battle      ",LED2_2,RED,7,POLY,-12,0},  //Button 6
  {BUTTON3_2,"Missle Launch!!!",LED3_2,RED,11,POLY,0,0},  //Button 7
  {BUTTON4_2,"Modern Battle fX",LED4_2,YELLOW,12,POLY,1,0},  //Button 8

  {BUTTON1_3,"Sounds Of War   ",LED1_3,RED,15,POLY,-8,0},  //Button 9
  {BUTTON2_3,"AntiAircraft Gun",LED2_3,RED,19,POLY,0,0},  //Button 10
  {BUTTON3_3,"Enemy Spotted!! ",LED3_3,YELLOW,18,POLY,4,0},   //Button 11
  {BUTTON4_3,"Roger           ",LED4_3,YELLOW,13,POLY,0,0},  //Button 12

  {BUTTON1_4,"Air Raid Siren  ",LED1_4,BLUE,3,POLY,-20,0},   //Button 13
  {BUTTON2_4,"Jet Fly Over    ",LED2_4,BLUE,22,POLY,0,0},   //Button 14
  {BUTTON3_4,"Morse Code ..-.-",LED3_4,BLUE,29,POLY,0,0},   //Button 15
  {BUTTON4_4,"Ship Comms      ",LED4_4,BLUE,34,POLY,0,0},  //Button 16

  //********************  Config #1: A10 Warthog  ********************************//
  {BUTTON1_1,"Odessy ThemeSong",LED1_1,BLUE,55,SINGLE,3,86},     //Button 1
  {BUTTON2_1,"BerverlyHillsCop",LED2_1,BLUE,73,SINGLE,3,181},    //Button 2
  {BUTTON3_1,"Indiana Jones   ",LED3_1,BLUE,68,SINGLE,0,118},      //Button 3
  {BUTTON4_1,"Dr. No  007 Song",LED4_1,BLUE,58,SINGLE,0,107},   //Button 4

  {BUTTON1_2,"HD Machine Gun!!",LED1_2,RED,8,POLY,0,0},   //Button 5
  {BUTTON2_2,"Warthog 0mmCanon",LED2_2,RED,4,POLY,100,},  //Button 6
  {BUTTON3_2,"Missle Launch!!!",LED3_2,GREEN,11,POLY,0,0},  //Button 7
  {BUTTON4_2,"Bomb Drop!!!!!!!",LED4_2,GREEN,2,POLY,8,0},  //Button 8

  {BUTTON1_3,"Grenade        ",LED1_3,RED,6,POLY,0,0},  //Button 9
  {BUTTON2_3,"Sounds of War  ",LED2_3,RED,15,POLY,-8,0},  //Button 10
  {BUTTON3_3,"Enemy Spotted!! ",LED3_3,YELLOW,18,POLY,4,0},   //Button 11
  {BUTTON4_3,"Roger           ",LED4_3,YELLOW,13,POLY,0,0},  //Button 12

  {BUTTON1_4,"Thunder        ",LED1_4,BLUE,16,POLY,0,0},   //Button 13
  {BUTTON2_4,"Group Applause ",LED2_4,BLUE,30,POLY,0,0},   //Button 14
  {BUTTON3_4,"Artillery SFx  ",LED3_4,BLUE,46,POLY,0,0},   //Button 15
  {BUTTON4_4,"FireEngineSiren",LED4_4,BLUE,25,POLY,-6,0},    //Button 16

  //********************  Config #2: Aerobatics  ********************************//
  {BUTTON1_1,"DJKilled Mix    ",LED1_1,BLUE,62,SINGLE,0,395},     //Button 1
  {BUTTON2_1,"Dr. No  007 Song",LED2_1,GREEN,58,SINGLE,0,107},    //Button 2
  {BUTTON3_1,"DreamMachine DJ ",LED3_1,RED,65,SINGLE,0,300},      //Button 3
  {BUTTON4_1,"ModernI         ",LED4_1,YELLOW,24,SINGLE,0,232},   //Button 4

  {BUTTON1_2,"Applause Woh!!!!",LED1_2,BLUE,21,POLY,0,0},   //Button 5
  {BUTTON2_2,"StarTrek Deni Fx",LED2_2,RED,63,POLY,0,0},  //Button 6
  {BUTTON3_2,"StarTrek Deni!! ",LED3_2,GREEN,64,POLY,0,0},  //Button 7
  {BUTTON4_2,"Club Mix DJ Song",LED4_2,YELLOW,59,SINGLE,0,0},  //Button 8

  {BUTTON1_3,"Unicom Song     ",LED1_3,BLUE,57,SINGLE,0,0},  //Button 9
  {BUTTON2_3,"F32 DJ          ",LED2_3,BLUE,54,SINGLE,0,0},  //Button 10
  {BUTTON3_3,"DJ Jet Mix      ",LED3_3,BLUE,47,SINGLE,0,0},  //Button 11
  {BUTTON4_3,"Paul Remix      ",LED4_3,BLUE,41,SINGLE,0,0},   //Button 12

  {BUTTON1_4,"DJ ExtremeTechno",LED1_4,BLUE,26,SINGLE,0,0},   //Button 13
  {BUTTON2_4,"Patch Instrument",LED2_4,BLUE,50,SINGLE,0,0},   //Button 14
  {BUTTON3_4,"Applause Group  ",LED3_4,BLUE,30,POLY,0,0},   //Button 15
  {BUTTON4_4,"Benny Hill Song ",LED4_4,BLUE,23,POLY,0,0},    //Button 16

  //********************  Config #3: Warbirds  ********************************//
  {BUTTON1_1,"BaaBaaBlacksheep",LED1_1,BLUE,74,SINGLE,0,42},     //Button 1
  {BUTTON2_1,"Odessy ThemeSong",LED2_1,GREEN,55,SINGLE,0,86},    //Button 2
  {BUTTON3_1,"Army Strong Song",LED3_1,RED,20,SINGLE,3,130},      //Button 3
  {BUTTON4_1,"VA Military Song",LED4_1,YELLOW,63,SINGLE,0,60},   //Button 4

  {BUTTON1_2,"HD Machine Gun!!",LED1_2,BLUE,8,POLY,0,0},   //Button 5
  {BUTTON2_2,"Air Raid Siren!!",LED2_2,RED,3,POLY,-5,0},  //Button 6
  {BUTTON3_2,"Gun Battle      ",LED3_2,GREEN,7,POLY,-6,0},  //Button 7
  {BUTTON4_2,"Modern Battle fX",LED4_2,YELLOW,12,POLY,0,0},  //Button 8

  {BUTTON1_3,"Sounds Of War   ",LED1_3,BLUE,15,POLY,0,0},  //Button 9
  {BUTTON2_3,"AntiAircraft Gun",LED2_3,BLUE,19,POLY,0,0},  //Button 10
  {BUTTON3_3,"Grenade         ",LED3_3,BLUE,6,POLY,0,0},  //Button 11
  {BUTTON4_3,"Glock 18FA      ",LED4_3,BLUE,5,POLY,0,0},   //Button 12

  {BUTTON1_4,"Roger           ",LED1_4,BLUE,13,POLY,0,0},   //Button 13
  {BUTTON2_4,"Enemy Spotted!! ",LED2_4,BLUE,18,POLY,0,0},   //Button 14
  {BUTTON3_4,"Artillery Fx    ",LED3_4,BLUE,46,POLY,0,0},   //Button 15
  {BUTTON4_4,"Porky Pig Swear!",LED4_4,BLUE,39,POLY,0,0},    //Button 16

  //********************  Config #4: Lawn Mowers  ********************************//
//  {BUTTON1_1,"MakingToys Instr",LED1_1,BLUE,51,SINGLE,0,243},     //Button 1
//  {BUTTON2_1,"Benny Hill x1   ",LED2_1,GREEN,23,SINGLE,0,62},    //Button 2
//  {BUTTON3_1,"Indiana Jones   ",LED3_1,RED,68,SINGLE,0,118},      //Button 3
//  {BUTTON4_1,"Hogan's Hero's  ",LED4_1,YELLOW,74,SINGLE,0,42},   //Button 4

  {BUTTON1_1,"MakingToys Instr",LED1_1,BLUE,51,SINGLE,0,236},      //Button 1
  {BUTTON2_1,"Hogan's Hero's  ",LED2_1,YELLOW,74,SINGLE,0,42},    //Button 2
  {BUTTON3_1,"Benny Hill x1   ",LED3_1,GREEN,23,SINGLE,0,62},     //Button 3
  {BUTTON4_1,"MakingToys Instr",LED4_1,BLUE,51,SINGLE,0,236},  //Button 4

  {BUTTON1_2,"Silly Snoring   ",LED1_2,BLUE,14,POLY,0,0},   //Button 5
  {BUTTON2_2,"Dog Bark        ",LED2_2,RED,37,POLY,-5,0},  //Button 6
  {BUTTON3_2,"Cat Meow        ",LED3_2,GREEN,38,POLY,0,0},  //Button 7
  {BUTTON4_2,"Monster         ",LED4_2,YELLOW,35,POLY,0,0},  //Button 8

  {BUTTON1_3,"Fire Engine Strt",LED1_3,BLUE,25,POLY,0,0},  //Button 9
  {BUTTON2_3,"Fire Engine Stp ",LED2_3,BLUE,28,POLY,0,0},  //Button 10
  {BUTTON3_3,"Grenade         ",LED3_3,BLUE,6,POLY,0,0},  //Button 11
  {BUTTON4_3,"Glock 18FA      ",LED4_3,BLUE,5,POLY,0,0},   //Button 12

  {BUTTON1_4,"Engine Fail Heli",LED1_4,BLUE,33,POLY,0,0},   //Button 13
  {BUTTON2_4,"Applause Group  ",LED2_4,BLUE,30,POLY,0,0},   //Button 14
  {BUTTON3_4,"Applause Woh!!!!",LED3_4,BLUE,21,POLY,0,0},   //Button 15
  {BUTTON4_4,"Heli Approach   ",LED4_4,BLUE,48,POLY,0,0},    //Button 16

  //********************  Config #5: Turbine Glider  **************************//
  {BUTTON1_1,"BerverlyHillsCop",LED1_1,BLUE,73,SINGLE,0,181},     //Button 1
  {BUTTON2_1,"BostonMoreFeelng",LED2_1,GREEN,67,SINGLE,0,205},    //Button 2
  {BUTTON3_1,"VanHalen Dreams ",LED3_1,RED,69,SINGLE,0,294},      //Button 3
  {BUTTON4_1,"Applause Woh!!!!",LED4_1,YELLOW,21,SINGLE,0,8},   //Button 4

  {BUTTON1_2,"Silly Snoring   ",LED1_2,BLUE,14,POLY,0,0},   //Button 5
  {BUTTON2_2,"Dog Bark        ",LED2_2,RED,37,POLY,-5,0},  //Button 6
  {BUTTON3_2,"Cat Meow        ",LED3_2,GREEN,38,POLY,0,0},  //Button 7
  {BUTTON4_2,"Monster         ",LED4_2,YELLOW,35,POLY,0,0},  //Button 8

  {BUTTON1_3,"Fire Engine Strt",LED1_3,BLUE,25,POLY,0,0},  //Button 9
  {BUTTON2_3,"Fire Engine Stp ",LED2_3,BLUE,28,POLY,0,0},  //Button 10
  {BUTTON3_3,"Grenade         ",LED3_3,BLUE,6,POLY,0,0},  //Button 11
  {BUTTON4_3,"Glock 18FA      ",LED4_3,BLUE,5,POLY,0,0},   //Button 12

  {BUTTON1_4,"Engine Fail Heli",LED1_4,BLUE,33,POLY,0,0},   //Button 13
  {BUTTON2_4,"Xformers SFx    ",LED2_4,BLUE,45,POLY,0,0},   //Button 14
  {BUTTON3_4,"Sound Fx Future ",LED3_4,BLUE,44,POLY,0,0},   //Button 15
  {BUTTON4_4,"Heli Approach   ",LED4_4,BLUE,48,POLY,0,0},    //Button 16

  //********************  Config #6: J10 Jet   **************************//
  {BUTTON1_1,"Odessy ThemeSong",LED1_1,BLUE,55,SINGLE,3,86},     //Button 1
  {BUTTON2_1,"BerverlyHillsCop",LED2_1,BLUE,73,SINGLE,3,181},    //Button 2
  {BUTTON3_1,"Indiana Jones   ",LED3_1,BLUE,68,SINGLE,0,118},      //Button 3
  {BUTTON4_1,"Dr. No  007 Song",LED4_1,BLUE,58,SINGLE,0,107},   //Button 4

  {BUTTON1_2,"HD Machine Gun!!",LED1_2,RED,8,POLY,0,0},   //Button 5
  {BUTTON2_2,"Warthog 0mmCanon",LED2_2,RED,4,POLY,100,},  //Button 6
  {BUTTON3_2,"Missle Launch!!!",LED3_2,GREEN,11,POLY,0,0},  //Button 7
  {BUTTON4_2,"Bomb Drop!!!!!!!",LED4_2,GREEN,2,POLY,8,0},  //Button 8

  {BUTTON1_3,"Grenade        ",LED1_3,RED,6,POLY,0,0},  //Button 9
  {BUTTON2_3,"Sounds of War  ",LED2_3,RED,15,POLY,-8,0},  //Button 10
  {BUTTON3_3,"Enemy Spotted!! ",LED3_3,YELLOW,18,POLY,4,0},   //Button 11
  {BUTTON4_3,"Roger           ",LED4_3,YELLOW,13,POLY,0,0},  //Button 12

  {BUTTON1_4,"Applause Woh!!!!",LED1_4,YELLOW,21,SINGLE,0,8},   //Button 4
  {BUTTON2_4,"Xformers SFx    ",LED2_4,BLUE,45,POLY,0,0},   //Button 14
  {BUTTON3_4,"Sound Fx Future ",LED3_4,BLUE,44,POLY,0,0},   //Button 15
  {BUTTON4_4,"Heli Approach   ",LED4_4,BLUE,48,POLY,0,0}    //Button 16

  };


void setup() {

Serial3.begin(57600);  //Serial Comms to WAV Trigger
Serial2.begin(9600);   //Serial Comms to LCD
delay(500); // wait for display to boot up

//Setup first back of outputs for LEDs ON/OFF control
// 1= LED OFF   0=LED ON
pinMode(LED1_1, OUTPUT);
pinMode(LED2_1, OUTPUT);
pinMode(LED3_1, OUTPUT);
pinMode(LED4_1, OUTPUT);

pinMode(LED1_2, OUTPUT);
pinMode(LED2_2, OUTPUT);
pinMode(LED3_2, OUTPUT);
pinMode(LED4_2, OUTPUT);

pinMode(LED1_3, OUTPUT);
pinMode(LED2_3, OUTPUT);
pinMode(LED3_3, OUTPUT);
pinMode(LED4_3, OUTPUT);

pinMode(LED1_4, OUTPUT);
pinMode(LED2_4, OUTPUT);
pinMode(LED3_4, OUTPUT);
pinMode(LED4_4, OUTPUT);

//Setup for RBG color control
// 0=Color OFF  1=Color ON
pinMode(GREEN1, OUTPUT);  //Green
pinMode(BLUE1, OUTPUT);  //Blue
pinMode(RED1, OUTPUT);  //Red

pinMode(GREEN2, OUTPUT);  //Green
pinMode(BLUE2, OUTPUT);  //Blue
pinMode(RED2, OUTPUT);  //Red

pinMode(GREEN3, OUTPUT);  //Green
pinMode(BLUE3, OUTPUT);  //Blue
pinMode(RED3, OUTPUT);  //Red

pinMode(GREEN4, OUTPUT);  //Green
pinMode(BLUE4, OUTPUT);  //Blue
pinMode(RED4, OUTPUT);  //Red

//Setup Pushbuttons for read and pull resistors
pinMode(BUTTON1_1, INPUT);
pinMode(BUTTON2_1, INPUT);
pinMode(BUTTON3_1, INPUT);
pinMode(BUTTON4_1, INPUT);
digitalWrite(BUTTON1_1, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON2_1, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON3_1, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON4_1, HIGH);  // Turn on internal Pull-Up Resistor

pinMode(BUTTON1_2, INPUT);
pinMode(BUTTON2_2, INPUT);
pinMode(BUTTON3_2, INPUT);
pinMode(BUTTON4_2, INPUT);
digitalWrite(BUTTON1_2, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON2_2, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON3_2, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON4_2, HIGH);  // Turn on internal Pull-Up Resistor

pinMode(BUTTON1_3, INPUT);
pinMode(BUTTON2_3, INPUT);
pinMode(BUTTON3_3, INPUT);
pinMode(BUTTON4_3, INPUT);
digitalWrite(BUTTON1_3, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON2_3, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON3_3, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON4_3, HIGH);  // Turn on internal Pull-Up Resistor

pinMode(BUTTON1_4, INPUT);
pinMode(BUTTON2_4, INPUT);
pinMode(BUTTON3_4, INPUT);
pinMode(BUTTON4_4, INPUT);
digitalWrite(BUTTON1_4, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON2_4, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON3_4, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(BUTTON4_4, HIGH);  // Turn on internal Pull-Up Resistor

pinMode(Change_CFG_BUTTON, INPUT);
pinMode(STOP_ALL_BUTTON, INPUT);
pinMode(FOOT_SW1, INPUT);
pinMode(FOOT_SW2, INPUT);
pinMode(FOOT_SW3, INPUT);

digitalWrite(Change_CFG_BUTTON, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(STOP_ALL_BUTTON, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(FOOT_SW1, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(FOOT_SW2, HIGH);  // Turn on internal Pull-Up Resistor
digitalWrite(FOOT_SW3, HIGH);  // Turn on internal Pull-Up Resistor


//Start with no color selected
  AllColorPinsOff();

//Start with all LEDs OFF
  All_LEDsOff();

//Display Splash Screen
  SplashScreen();

//LED Test Patterns
  LED_Test();

//delay(3000);
  All_LEDsOff();
  AllColorPinsOff();

  t.every(1000,requestWAV_Status);

  //Initial configuration is F-16
  SendLCD_Text("F16 Falcon      Cfg Selected");
  delay(1500);
}

void loop()
{
  int packet[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  int x,y, num_wavs_playing = 0;
  bool buttonState,buttonState3,buttonState4;

  t.update();

  buttonState = digitalRead(STOP_ALL_BUTTON);
  if( !buttonState )
     stopAll_WAVs();

  buttonState = digitalRead(Change_CFG_BUTTON);
  if( !buttonState )
  {
     ConfigurationNumber++;
     if( ConfigurationNumber == NUM_CONFIGS )
       ConfigurationNumber = 0;

     stopAll_WAVs();

     if (ConfigurationNumber == 0)
        SendLCD_Text("F16 Falcon      Cfg Selected");
     else if( ConfigurationNumber == 1)
        SendLCD_Text("Warthog         Cfg Selected");
     else if( ConfigurationNumber == 2)
        SendLCD_Text("Aerobatic       Cfg Selected");
     else if( ConfigurationNumber == 3)
        SendLCD_Text("Warbird         Cfg Selected");
     else if( ConfigurationNumber == 4)
        SendLCD_Text("Lawn Mowers     Cfg Selected");
     else if( ConfigurationNumber == 5)
        SendLCD_Text("Turbine Glider  Cfg Selected");

     delay(1000);
     SendLCD_Text("                            ");
  }

  //Read foot sw1 and kick off song #1 of current cfg
  buttonState = digitalRead(FOOT_SW1);
  if( !buttonState )  
  {
     trackStart_secs = now();
     CurrentPlayingTrackLength = KeyConfig[16*ConfigurationNumber+FootSWSongNumber].Track_Duration_Secs;
     CurrentPlayingTrackTitle = KeyConfig[16*ConfigurationNumber+FootSWSongNumber].LCD_Msg;
     SendLCD_Text(KeyConfig[16*ConfigurationNumber+FootSWSongNumber].LCD_Msg);
     play_track(KeyConfig[16*ConfigurationNumber+FootSWSongNumber].TrackNumber,KeyConfig[16*ConfigurationNumber+FootSWSongNumber].SinglePoly,KeyConfig[16*ConfigurationNumber+FootSWSongNumber].Volume);
     LED_On( KeyConfig[16*ConfigurationNumber+FootSWSongNumber].LED_DO_Num, KeyConfig[16*ConfigurationNumber+FootSWSongNumber].Color );   

     //Play the next song next time the Song foot switch is pressed.
     FootSWSongNumber++;
     SendLCD_Num(FootSWSongNumber);
     if( FootSWSongNumber == 4 )
       FootSWSongNumber = 0;
  }

  //Read foot sw2 and kick off Effect #1 of current cfg
  buttonState = digitalRead(FOOT_SW2);
  if( !buttonState )  
  {
     SendLCD_Text(KeyConfig[16*ConfigurationNumber+4].LCD_Msg);
     play_track(KeyConfig[16*ConfigurationNumber+4].TrackNumber,KeyConfig[16*ConfigurationNumber+4].SinglePoly,KeyConfig[16*ConfigurationNumber+4].Volume);
     LED_On( KeyConfig[16*ConfigurationNumber+4].LED_DO_Num, KeyConfig[16*ConfigurationNumber+4].Color );    
  }  

  //Read foot sw3 and kick off Effect #2 of current cfg
  buttonState = digitalRead(FOOT_SW3);
  if( !buttonState )  
  {
     SendLCD_Text(KeyConfig[16*ConfigurationNumber+5].LCD_Msg);
     play_track(KeyConfig[16*ConfigurationNumber+5].TrackNumber,KeyConfig[16*ConfigurationNumber+5].SinglePoly,KeyConfig[16*ConfigurationNumber+5].Volume);
     LED_On( KeyConfig[16*ConfigurationNumber+5].LED_DO_Num, KeyConfig[16*ConfigurationNumber+5].Color );    
  } 
  
  //Check upper left and right buttons presses together...if so enable or disable the internal speaker!
  buttonState3 = digitalRead(BUTTON1_1);
  buttonState4 = digitalRead(BUTTON2_3);
  if( !buttonState3 && !buttonState4)
  {
     AmpEnabled = !AmpEnabled;
    if(AmpEnabled)
    {
        Serial3.write(msg_wt_enableamp, sizeof(msg_wt_enableamp));
        SendLCD_Text("External Speaker    Enabled!!   ");
    }
    else
    {
        Serial3.write(msg_wt_disableamp, sizeof(msg_wt_disableamp));
        SendLCD_Text("External Speaker   Disabled!!   ");
    }
    delay(1500);
    SendLCD_Text(CurrentPlayingTrackTitle);
  }

  //Scan the 16 white rubber buttons for input...
  for (x=0+(16*ConfigurationNumber); x<MAX_BUTTONS+(16*ConfigurationNumber); x++)
  {

     buttonState = digitalRead(KeyConfig[x].Button_DI_Num);

     if( !buttonState )
     {
         //If a button is pressed in the first group of four buttons...get the current secs so that later we can flag with end of song warnings...
         if( (KeyConfig[x].Button_DI_Num == BUTTON1_1) || (KeyConfig[x].Button_DI_Num == BUTTON2_1) ||(KeyConfig[x].Button_DI_Num == BUTTON3_1) ||(KeyConfig[x].Button_DI_Num == BUTTON4_1) )
         {
           trackStart_secs = now();
           CurrentPlayingTrackLength = KeyConfig[x].Track_Duration_Secs;
           CurrentPlayingTrackTitle = KeyConfig[x].LCD_Msg;
         }

         SendLCD_Text(KeyConfig[x].LCD_Msg);
         play_track(KeyConfig[x].TrackNumber,KeyConfig[x].SinglePoly,KeyConfig[x].Volume);
         LED_On( KeyConfig[x].LED_DO_Num, KeyConfig[x].Color );
     }
  }

  //See if the WAV trigger has responded with any tracks playing...if so update the display and such.
  if(int num_bytes = Serial3.available())
  {
     // read the incoming data...
     for( x=0; x<= num_bytes; x++)
       packet[x] = Serial3.read();

     //Clear all tracks_playing and then repopulate with latest....if any
     for(x=0; x<MAX_WAVS_PLAYING; x++)
        tracks_playing[x] = 0;
     //Check for WAVs playing
     if( packet[2] != 5 )
     {
        num_wavs_playing = (packet[2]-5)/2;
        for( x=0; x<num_wavs_playing; x++ )
           tracks_playing[x] = packet[(4+x*2)];
     }
  }


    //Turn ON LEDs that are playing based on the Status msg from the WAV Trigger...
    for( x=0; x<MAX_WAVS_PLAYING; x++ )
    {
       if( tracks_playing[x] == 0 )
       {
          break;
       }
       else
       {
          //Look up which button is mapped to this WAV file
          for(y=0+(16*ConfigurationNumber); y<MAX_BUTTONS+(16*ConfigurationNumber); y++)
            if( KeyConfig[y].TrackNumber == tracks_playing[x])
            {
               LED_On(KeyConfig[y].LED_DO_Num,KeyConfig[y].Color);
            }
       }
    }

    //Check and flag the user if the end of a song is coming up!!
    if(now() == (trackStart_secs+CurrentPlayingTrackLength-30) )
    {
        SendLCD_Text("30 seconds till song end!!");
    }
    if(now() == (trackStart_secs+CurrentPlayingTrackLength-15) )
        SendLCD_Text("15 seconds till song end!!");
    if(now() == (trackStart_secs+CurrentPlayingTrackLength-5) )
        SendLCD_Text("5 seconds till  song end!!");
    if( (now() == (trackStart_secs+CurrentPlayingTrackLength-29)) || (now() == (trackStart_secs+CurrentPlayingTrackLength-14)) || (now() == (trackStart_secs+CurrentPlayingTrackLength-4)) )
      SendLCD_Text(CurrentPlayingTrackTitle);


    //Wait a short delay to allow for any LEDs that are on to be seen...then turn them all off and go around again.
    delay(300);
    All_LEDsOff();
}

void requestWAV_Status( void )
{
  //Request Status from WAV Trigger...
  Serial3.write(msg_wt_get_status, sizeof(msg_wt_get_status));
}

void play_track( int track_number, int poly, int vol)
{
  //adjust volume...don't play just yet..
  msg_wt_trackvol[4] = track_number;
  msg_wt_trackvol[5] = 0x00;

  if( vol >= 0 )
  {
     msg_wt_trackvol[6] = vol;
     msg_wt_trackvol[7] = 0x00;
  }
  else
  {
     msg_wt_trackvol[6] = 0x00FF-(vol+1)*-1;
     msg_wt_trackvol[7] = 0xFF;
  }
  Serial3.write(msg_wt_trackvol, sizeof(msg_wt_trackvol));

  delay(200);

  if( poly )
    msg_wt_poly[4] = 0x01;
  else
    msg_wt_poly[4] = 0x00;

  //Play Track w/poly setting
  msg_wt_poly[5] = track_number;
  Serial3.write(msg_wt_poly, sizeof(msg_wt_poly));
}

void stopAll_WAVs( void )
{
  Serial3.write(msg_wt_stopall, sizeof(msg_wt_stopall));
}



void cfg_track( int track_number, int playback)
{
  msg_wt_single[5] = track_number-1;
  msg_wt_single[6] = 0x00;
  Serial3.write(msg_wt_single, sizeof(msg_wt_single));
}

void SendLCD_Num( int num_value)
{
Serial2.write(254); // move cursor to beginning of second line
  Serial2.write(192);
  Serial2.print(num_value,DEC);


}

void SendLCD_Text( String display_txt)
{

  Serial2.write("                "); // clear display
  Serial2.write("                ");

  Serial2.write(254); // move cursor to beginning of first line
  Serial2.write(128);
  Serial2.print(display_txt);
}

void SplashScreen( void )
{

  Serial2.write(254); // move cursor to beginning of first line
  Serial2.write(128);

  Serial2.write("                "); // clear display
  Serial2.write("                ");

  Serial2.write(254); // move cursor to beginning of first line
  Serial2.write(128);
  Serial2.write("  HOBBY HANGAR");
  LED_Chase(BLUE,5);
  int x;
  for( x=128; x<=143; x++ )
  {
     Serial2.write(254);
     Serial2.write(x);
     Serial2.write("  HOBBY HANGAR");
     delay(200);
  }
  LED_Chase(RED,6);
  Serial2.write(254); // move cursor to beginning of second line
  Serial2.write(192);
  Serial2.write("   AIRSHOW fX");
  LED_Chase(GREEN,8);
  Serial2.write(254); // move cursor to beginning of first line
  Serial2.write(128);
  Serial2.write("   AIRSHOW fX");
  Serial2.write(254); // move cursor to beginning of second line
  Serial2.write(192);
  Serial2.write("                ");
  LED_Chase(YELLOW,10);
}

void LED_Test( void)
{
  int x;

  for(x=0; x<MAX_BUTTONS; x++)
  {
     LED_On(LEDenable_Button[x],BLUE);
     delay(50);
     LED_On(LEDenable_Button[x],GREEN);
     delay(50);
     LED_On(LEDenable_Button[x],RED);
     delay(50);
     LED_On(LEDenable_Button[x],YELLOW);
     delay(50);
  }
}


void LED_Chase(int color, int how_fast)
{
  int x;
  //  speed is value 1-10...10 being fastest.
  for(x=0; x<MAX_BUTTONS; x++)
  {
     LED_On(Chase_Seq[x],color);
     delay(250/how_fast);
  }
}


void AllColorPinsOff( void )
{
  digitalWrite(BLUE1, LOW);
  digitalWrite(GREEN1, LOW);
  digitalWrite(RED1, LOW);

  digitalWrite(BLUE2, LOW);
  digitalWrite(GREEN2, LOW);
  digitalWrite(RED2, LOW);

  digitalWrite(BLUE3, LOW);
  digitalWrite(GREEN3, LOW);
  digitalWrite(RED3, LOW);

  digitalWrite(BLUE4, LOW);
  digitalWrite(GREEN4, LOW);
  digitalWrite(RED4, LOW);
}

void All_LEDsOff( void )
{
  digitalWrite(LED1_1, HIGH);
  digitalWrite(LED2_1, HIGH);
  digitalWrite(LED3_1, HIGH);
  digitalWrite(LED4_1, HIGH);

  digitalWrite(LED1_2, HIGH);
  digitalWrite(LED2_2, HIGH);
  digitalWrite(LED3_2, HIGH);
  digitalWrite(LED4_2, HIGH);

  digitalWrite(LED1_3, HIGH);
  digitalWrite(LED2_3, HIGH);
  digitalWrite(LED3_3, HIGH);
  digitalWrite(LED4_3, HIGH);

  digitalWrite(LED1_4, HIGH);
  digitalWrite(LED2_4, HIGH);
  digitalWrite(LED3_4, HIGH);
  digitalWrite(LED4_4, HIGH);
}

void LED_On( int someLED, int someColor )
{
  int someGroup =  0;

    //Check for Group Buttons...
    if( someLED >= 9 && someLED <= 12 )
      someGroup = 1;
    else if( someLED >= 5 && someLED <= 8 )
      someGroup = 2;
    else if( someLED >= 1 && someLED <= 4 )
      someGroup = 3;
    else if( someLED >= 13 && someLED <= 20 )
      someGroup = 4;

    //Drive the correct color pin
    driveColorPin( someColor, someGroup );

    //Enable the LEDs for this button...
    digitalWrite(someLED, LOW);
}

void LED_Update( int someButton, int buttonColor, int inGroup)
{

  //Turn it ON for 200 ms...
   digitalWrite(someButton, LOW);
   driveColorPin( buttonColor, inGroup);
   delay( 100 );
   //then turn it OFF
   digitalWrite(someButton, HIGH);
   driveColorPin( buttonColor, inGroup);
}

void driveColorPin( int buttonColor, int group)
{
  int gcB, gcG, gcR;

  switch (group)
  {
    case 1:
          gcB = BLUE1;
          gcG = GREEN1;
          gcR = RED1;
      break;
    case 2:
          gcB = BLUE2;
          gcG = GREEN2;
          gcR = RED2;
      break;
    case 3:
          gcB = BLUE3;
          gcG = GREEN3;
          gcR = RED3;
      break;
    case 4:
          gcB = BLUE4;
          gcG = GREEN4;
          gcR = RED4;
      break;

    default:
      // if nothing else matches, do the default
      // default is optional
    break;
  }

  switch (buttonColor)
  {
    case RED:
        digitalWrite(gcB, 0);
        digitalWrite(gcG, 0);
        digitalWrite(gcR, 1);
      break;
    case GREEN:
        digitalWrite(gcB, 0);
        digitalWrite(gcG, 1);
        digitalWrite(gcR, 0);
      break;
    case BLUE:
        digitalWrite(gcB, 1);
        digitalWrite(gcG, 0);
        digitalWrite(gcR, 0);
      break;
    case YELLOW:
        digitalWrite(gcB, 0);
        digitalWrite(gcR, 1);
        digitalWrite(gcG, 1);

      break;

    default:
      // if nothing else matches, do the default
      // default is optional
    break;
  }
}


//Some common debug used...
/*  Serial2.write(254); // move cursor to beginning of first line
  Serial2.write(128);
  Serial2.print(packet[0],HEX);
  Serial2.print(" ");
  Serial2.print(packet[1],HEX);
  Serial2.print(" ");
  Serial2.print(packet[2],HEX);
  Serial2.print(" ");
  Serial2.print(packet[3],HEX);
  Serial2.print(" ");
  Serial2.print(packet[4],HEX);
  Serial2.print(" ");
  Serial2.print(packet[5],HEX);*/


//  Serial2.write(254); // move cursor to beginning of first line
//  Serial2.write(128);
//  Serial2.print(tracks_playing[0]);
//  delay(500);

  /*  Serial2.write(254); // move cursor to beginning of second line
  Serial2.write(192);
  Serial2.print(num_wavs_playing,HEX);
  Serial2.print("::");
  Serial2.print(tracks_playing[0],HEX);
  Serial2.print(" ");
  Serial2.print(tracks_playing[1],HEX);
  Serial2.print(" ");
  Serial2.print(tracks_playing[2],HEX);
  Serial2.print(" ");
  Serial2.print(tracks_playing[3],HEX);
  Serial2.print(" ");
  Serial2.print(tracks_playing[4],HEX);
  Serial2.print(" ");
  Serial2.print(tracks_playing[5],HEX);*/

/*
void WAV_config( int config_type )
{
  switch (config_type)
  {
    case AC3:
        cfg_track( 1, SINGLE);
        cfg_track( 20, SINGLE);
        cfg_track( 15, SINGLE);
      break;
    case SFX:
        cfg_track( 1, SINGLE);
        cfg_track( 17, SINGLE);
        cfg_track( 20, SINGLE);
        cfg_track( 23, SINGLE);
      break;


    default:
      // if nothing else matches, do the default
      // default is optional
    break;
  }

}
*/

