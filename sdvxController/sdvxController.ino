#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <EEPROM.h>
#include "SDVXHID.h"

/* use encoders rather than potentiometers */
#define USE_ENCODERS

#ifdef USE_ENCODERS
  #define ENCODER_PPR 360
  #define ENCODER_SENSITIVITY (float) (1023/(float)ENCODER_PPR)
 // Button pins
#define START 4
#define BT_A  5
#define BT_B  6
#define BT_C  7
#define BT_D  8
#define FX_L  9
#define FX_R  10

// LED pins
#define LED_START A4
#define LED_A     A3
#define LED_B     A2
#define LED_C     A1
#define LED_D     A0
#define LED_FXL   11
#define LED_FXR   12
// SET LED_STRIP PIN DIRECTLY IN SDVXHID.h

#endif

/* 1 frame (as declared in SDVXHID.cpp) on fullspeed USB spec is 1ms */
#define REPORT_DELAY 1000
#define MILLIDEBOUNCE 5
SDVXHID_ SDVXHID;

#define REACTIVE_FALLBACK ((millis()-SDVXHID.getLastHidUpdate()) > 3000)

#ifdef USE_ENCODERS
uint8_t LightPins[] = {LED_START,LED_A,LED_B,LED_C,LED_D,LED_FXL,LED_FXR};
uint8_t ButtonPins[] = {START,BT_A,BT_B,BT_C,BT_D,FX_L,FX_R};
byte EncPins[]    = {0, 1, 2, 3};
byte EncPinCount = sizeof(EncPins) / sizeof(EncPins[0]);
#else
/* Buttons + Lights declarations */
uint8_t LightPins[] = {7,10,11,12,13,8,9};
//start a b c d fx-l fx-r service test
uint8_t ButtonPins[] = {0,3,4,5,6,1,2,A3,A2};
uint8_t PotPins[] = {A5,A4};
const byte PotCount = sizeof(PotPins) / sizeof(PotPins[0]);
#endif
//uint8_t RGBPins[] = {LED_STRIP_PIN_L,LED_STRIP_PIN_R}; //must be changed in SDVXHID.h

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
bool state[2]={false}, set[4]={false};

void doEncL(){
  if(state[0] == false && digitalRead(EncPins[0]) == LOW) {
    set[0] = digitalRead(EncPins[1]);
    state[0] = true;
  }
  if(state[0] == true && digitalRead(EncPins[0]) == HIGH) {
    set[1] = !digitalRead(EncPins[1]);
    if(set[0] == true && set[1] == true) {
      g_raw_encL++;
    }
    if(set[0] == false && set[1] == false) {
      g_raw_encL--;
    }
    state[0] = false;
  }
}

void doEncR(){
  if(state[1] == false && digitalRead(EncPins[2]) == LOW) {
    set[2] = digitalRead(EncPins[3]);
    state[1] = true;
  }
  if(state[1] == true && digitalRead(EncPins[2]) == HIGH) {
    set[3] = !digitalRead(EncPins[3]);
    if(set[2] == true && set[3] == true) {
      g_raw_encR++;
    }
    if(set[2] == false && set[3] == false) {
      g_raw_encR--;
    }
    state[1] = false;
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
for(int i=0;i<EncPinCount;i++) {
    pinMode(EncPins[i],INPUT_PULLUP);
  }
  attachInterrupt(digitalPinToInterrupt(EncPins[0]), doEncL, CHANGE);
  attachInterrupt(digitalPinToInterrupt(EncPins[2]), doEncR, CHANGE);
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
        g_raw_encL = 0;
    } else if (g_raw_encL < 0) {
        g_raw_encL = ENCODER_PPR - 1;
    }

    if (g_raw_encR >= ENCODER_PPR) {
        g_raw_encR = 0;
    } else if (g_raw_encR < 0) {
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
