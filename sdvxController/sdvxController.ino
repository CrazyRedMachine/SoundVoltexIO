#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <EEPROM.h>
#include "SDVXHID.h"
/* 1 frame (as declared in SDVXHID.cpp) on highspeed USB spec is 125µs */
#define REPORT_DELAY 1000
#define MILLIDEBOUNCE 5
SDVXHID_ SDVXHID;

/* Buttons + Lights declarations */
uint8_t LightPins[] = {7,10,11,12,13,8,9};
uint8_t ButtonPins[] = {0,3,4,5,6,1,2};
uint8_t PotPins[] = {A5,A4};
CRGB left_leds[SIDE_NUM_LEDS];
CRGB right_leds[SIDE_NUM_LEDS];

const byte ButtonCount = sizeof(ButtonPins) / sizeof(ButtonPins[0]);
const byte LightCount = sizeof(LightPins) / sizeof(LightPins[0]);
const byte PotCount = sizeof(PotPins) / sizeof(PotPins[0]);
Bounce buttons[ButtonCount];

/* SETUP */
void setup() {
  //Serial.begin(115200);
  // setup I/O for pins
  for (int i = 0; i < ButtonCount; i++) {
        buttons[i] = Bounce();
        buttons[i].attach(ButtonPins[i], INPUT_PULLUP);
        buttons[i].interval(MILLIDEBOUNCE);
  }

  for (int i = 0; i < LightCount; i++) {
    pinMode(LightPins[i], OUTPUT);
  }
  
  uint8_t lightMode;
  EEPROM.get(0, lightMode);
  if (lightMode < 0 || lightMode > 3)
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
uint32_t prevButtonsState = 0;
int32_t encL = 0;
int32_t encR = 0;

bool modeChanged = false;
void loop() {
  /* BUTTONS */
  uint32_t buttonsState = 0;
  int32_t delta;

  encL = analogRead(PotPins[0]);
  encR = analogRead(PotPins[1]);
  
  for (int i = 0; i < ButtonCount; i++) {
       buttons[i].update();
       int value = buttons[i].read();   
       int rawValue = digitalRead(ButtonPins[i]);   
    if ( value == LOW ){
      buttonsState |= (uint32_t)1 << i;
    } else {
      buttonsState &= ~((uint32_t)1 << i);
    }
  }

  /* USB DATA */
  if ( ( (micros() - lastReport) >= REPORT_DELAY) )
  {
   // Serial.print("will send value buttonstate = ");
   // Serial.print(buttonsState);
    SDVXHID.sendState(buttonsState, encL, encR);
    lastReport = micros();
    prevButtonsState = buttonsState; 
    
    //check for HID-requested lightmode change
    //SDVXHID.updateLightMode();
  }  
  
  /* LAMPS */
  uint8_t mode = SDVXHID.getLightMode();
  /* mixed mode will behave sometimes like HID, sometimes like reactive */
  if (mode == 2){
      if ((millis()-SDVXHID.getLastHidUpdate()) > 3000)
        mode = 0;
      else
        mode = 1;
  }
  switch (mode)
  {
    /* Reactive mode, locally determined lamp data */
    case 0:
      but_lights(buttonsState & 0x1ff, encL, encR, true);
      break;
    /* HID mode, only based on received HID data */
    case 1:
      SDVXHID.updateLeds(0, encL, encR, false);
      break;
    /* Combined inverse mode, received HID data and button state are combined then inverted */
    case 4:
      SDVXHID.updateLeds(buttonsState & 0x1ff, encL, encR, true);
      break;
    /* Combined mode, received HID data and button state are combined */
    case 3:
      SDVXHID.updateLeds(buttonsState & 0x1ff, encL, encR, false);
      break;
    default:
      break;
  }

  /* MANUAL LIGHTMODE UPDATE */
 /* if ( buttonsState & 1024 ) {
    if ( (buttonsState & 2) && (modeChanged == false)) {
      modeChanged = true;
      uint8_t mode = SDVXHID.getLightMode()+1;
      if (mode > 4) mode = 0;
      SDVXHID.setLightMode(mode);
      EEPROM.put(0, mode);
    }
    else if (!(buttonsState&2)) {
      modeChanged = false;
    }
  }*/
}

/* Light up button lights according to bitfield */
void but_lights(uint16_t lightDesc, uint32_t encL, uint32_t encR, bool use_knob) {
  static uint8_t circbuff_encL[5] = {0,0,0,0,0};
  static uint32_t prev_encL = 0;
  static int8_t encL_wp = 0;
  static int8_t sumL = 0; 
  
  static uint8_t circbuff_encR[5] = {0,0,0,0,0};
  static uint32_t prev_encR = 0;
  static int8_t encR_wp = 0;
  static int8_t sumR = 0; 
  
  static uint16_t blue = 0;
  static uint16_t red = 0;

if (use_knob){
  int val;
  if (encL == prev_encL) val = 0;
  else if (encL > prev_encL || prev_encL - encL > 1000 ) val = 1;
  else val = -1;

  sumL += -1*circbuff_encL[encL_wp];
  sumL += val;
  circbuff_encL[encL_wp++] = val;
  if (encL_wp == 5) encL_wp = 0;

  if (encR == prev_encR) val = 0;
  else if (encR > prev_encR || prev_encR - encR > 1000 ) val = 1;
  else val = -1;
  
  sumR += -1*circbuff_encR[encR_wp];
  sumR += val;
  circbuff_encR[encR_wp++] = val;
  if (encR_wp == 5) encR_wp = 0;

    /* compute blue and red shift */
//left knob
if (sumL > 4 || sumL < -4){
    if (blue<508) {
      blue+=2;
    }
    else blue = 510;
  } else if (sumL < 2 && sumL > -2){
    if (blue > 0) blue--;
  }
  prev_encL = encL;

//right knob
  if (sumR > 4 || sumR < -4){
    if (red<508) {
      red+=2;
    }
    else red = 510;
  } else if (sumR < 2 && sumR > -2){
    if (red > 0) red--;
  }
  prev_encR = encR;
  
  
  //apply light
      for (int i=0; i<9;i++){
  left_leds[i].setRGB(red/4, 0, blue/2);
  right_leds[i].setRGB(red/2, 0, blue/4);
      }
      
      FastLED.show();
}

  for (int i = 0; i < LightCount; i++) {
    if ((lightDesc >> i) & 1) {
      digitalWrite(LightPins[i], HIGH);
    }    else  {
      digitalWrite(LightPins[i], LOW);
    }
  }
}

/* Display animation on the cab according to a bitfield array */
void animate(uint16_t* tab, uint8_t n, int mswait) {
  for (int i = 0; i < n; i++) {
    but_lights(tab[i], 0, 0, false);
    delay(mswait);
  }
}
