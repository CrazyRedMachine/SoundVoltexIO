#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <EEPROM.h>
#include "SDVXHID.h"

/* use encoders rather than potentiometers */
#define USE_ENCODERS 1

#ifdef USE_ENCODERS
  #define ENCODER_PPR 400
  #define ENCODER_SENSITIVITY 1023/ENCODER_PPR
  #define ENC_L_A 0
  #define ENC_L_B 1
  #define ENC_L_B_ADDR 3
  #define ENC_R_A 2
  #define ENC_R_B 3
  #define ENC_R_B_ADDR 0
  #define ENCODER_PORT PIND
/* 
 * connect encoders
 * VOL-L to pins 0 and 1
 * VOL-R to pins 2 and 3
 */
#endif

/* 1 frame (as declared in SDVXHID.cpp) on fullspeed USB spec is 1ms */
#define REPORT_DELAY 1000
#define MILLIDEBOUNCE 5
SDVXHID_ SDVXHID;

#define REACTIVE_FALLBACK ((millis()-SDVXHID.getLastHidUpdate()) > 3000)

#ifdef USE_ENCODERS
uint8_t LightPins[] = {A4,A3,A2,A1,A0,11,12};
uint8_t ButtonPins[] = {4,5,6,7,8,9,10};
#else
/* Buttons + Lights declarations */
uint8_t LightPins[] = {7,10,11,12,13,8,9};
//start a b c d fx-l fx-r service test
uint8_t ButtonPins[] = {0,3,4,5,6,1,2,A3,A2};
uint8_t PotPins[] = {A5,A4};
const byte PotCount = sizeof(PotPins) / sizeof(PotPins[0]);
#endif
//uint8_t RGBPins[] = {A1,A0}; //must be changed directly inside SDVXHID.cpp in initRGB() method

CRGB left_leds[LEFT_NUM_LEDS]; //will be twice as big as SIDE_NUM_LEDS for single strip variant
#ifdef SINGLE_STRIP
CRGB *right_leds = &(left_leds[SIDE_NUM_LEDS]);
#else
CRGB right_leds[SIDE_NUM_LEDS];
#endif

const byte ButtonCount = sizeof(ButtonPins) / sizeof(ButtonPins[0]);
const byte LightCount = sizeof(LightPins) / sizeof(LightPins[0]);
Bounce buttons[ButtonCount];

#ifdef USE_ENCODERS
int32_t g_raw_encL = 0;
int32_t g_raw_encR = 0;

void doEncL(){
  if((ENCODER_PORT >> ENC_L_B_ADDR)&1){
    g_raw_encL++;
  } else {
    g_raw_encL--;
  }
}

void doEncR(){
  if((ENCODER_PORT >> ENC_R_B_ADDR)&1){
    g_raw_encR++;
  } else {
    g_raw_encR--;
  }
}
#endif

/* SETUP */
void setup() {
  DEBUG_INIT();
  // setup I/O for pins
  for (int i = 0; i < ButtonCount; i++) {
        buttons[i] = Bounce();
        buttons[i].attach(ButtonPins[i], INPUT_PULLUP);
        buttons[i].interval(MILLIDEBOUNCE);
  }

  for (int i = 0; i < LightCount; i++) {
    pinMode(LightPins[i], OUTPUT);
  }

  /**/
  #ifdef USE_ENCODERS
//setup interrupts
  pinMode(ENC_L_A,INPUT_PULLUP);
  pinMode(ENC_L_B,INPUT_PULLUP);
  pinMode(ENC_R_A,INPUT_PULLUP);
  pinMode(ENC_R_B,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_L_A), doEncL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_R_A), doEncR, RISING);
  #endif
  
  uint8_t lightMode;
  EEPROM.get(0, lightMode);
  if (lightMode >= NUM_LIGHT_MODES)
    lightMode = 2;
  SDVXHID.setLightMode(lightMode);

  SDVXHID.initRGB();
  
  //boot animation
  SDVXHID.setRGB(CRGB::Blue, CRGB::Blue);
  uint16_t anim[] = {1, 2, 4, 8, 16, 64, 32, 0};
  animate(anim, 8, 100);
  animate(anim, 8, 100);
  SDVXHID.setRGB(CRGB::Red, CRGB::Red);
  uint16_t anim2[] = {2+4+8+16, 1+32+64};
  animate(anim2, 2, 500);
  animate(anim2, 2, 500);
  SDVXHID.setRGB(0, 0);
}

/* LOOP */
unsigned long lastReport = 0;

bool modeChanged = false;
void loop() {
  /* BUTTONS */
  uint32_t buttonsState = 0;
  int32_t encL = 0;
  int32_t encR = 0;

#ifdef USE_ENCODERS
// Limit the encoder from 0 to ENCODER_PPR
    if (g_raw_encL >= ENCODER_PPR) {
        g_raw_encL = 1;
    } else if (g_raw_encL <= 0) {
        g_raw_encL = ENCODER_PPR - 1;
    }

    if (g_raw_encR >= ENCODER_PPR) {
        g_raw_encR = 1;
    } else if (g_raw_encR <= 0) {
        g_raw_encR = ENCODER_PPR - 1;
    }
    
  encL = g_raw_encL * ENCODER_SENSITIVITY;
  encR = g_raw_encR * ENCODER_SENSITIVITY;
#else
  encL = analogRead(PotPins[0]);
  encR = analogRead(PotPins[1]);
#endif

  for (int i = 0; i < ButtonCount; i++) {
       buttons[i].update();
       int value = buttons[i].read();  
    if ( value == LOW ){
      buttonsState |= (uint32_t)1 << i;
    } else {
      buttonsState &= ~((uint32_t)1 << i);
    }
  }

  /* USB DATA */
  if ( ( (micros() - lastReport) >= REPORT_DELAY) )
  {
    SDVXHID.sendState(buttonsState, encL, encR);
    lastReport = micros();
  }  
  
  /* LAMPS */
  uint8_t mode = SDVXHID.getLightMode();
  /* mixed mode will behave sometimes like HID, sometimes like reactive */
  if (mode == 2){
      if (REACTIVE_FALLBACK)
        mode = 0;
      else
        mode = 1;
  }
  switch (mode)
  {
    /* Reactive mode, locally determined lamp data */
    case 0:
      SDVXHID.updateLeds(buttonsState & 0x1ff, false, true, false);
      break;
    /* HID mode, only based on received HID data */
    case 1:
      SDVXHID.updateLeds(0, false, false, true);
      break;
    /* Combined inverse mode, received HID data and button state are combined then inverted */
    case 4:
      SDVXHID.updateLeds(buttonsState & 0x1ff, true, true, true);
      break;
    /* Combined mode, received HID data and button state are combined */
    case 3:
      SDVXHID.updateLeds(buttonsState & 0x1ff, false, true, !REACTIVE_FALLBACK);
      break;
    /* Reactive Rainbow edition */
    case 5:
      SDVXHID.rainbowLeds(buttonsState & 0x1ff);
      break;    
    /* Fake TC edition */
    case 6:
      SDVXHID.tcLeds(buttonsState & 0x1ff);
      break;
    default:
      break;
  }

  /* MANUAL LIGHTMODE UPDATE */
  if ( buttonsState & 128 ) {
    if ( (buttonsState & 2) && (modeChanged == false)) {
      modeChanged = true;
      uint8_t mode = SDVXHID.getLightMode()+1;
      if (mode == NUM_LIGHT_MODES) mode = 0;
      SDVXHID.setLightMode(mode);
      EEPROM.put(0, mode);
    }
    else if (!(buttonsState&2)) {
      modeChanged = false;
    }
  }
}

/* Display animation on the cab according to a bitfield array */
void animate(uint16_t* tab, uint8_t n, int mswait) {
  for (int i = 0; i < n; i++) {
    SDVXHID.updateLeds(tab[i], false, false, false);
    delay(mswait);
  }
}
