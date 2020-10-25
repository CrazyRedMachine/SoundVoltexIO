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
uint8_t ButtonPins[] = {0,3,4,5,6,16,2};
uint8_t PotPins[] = {A5,A4};

const byte ButtonCount = sizeof(ButtonPins) / sizeof(ButtonPins[0]);
const byte LightCount = sizeof(LightPins) / sizeof(LightPins[0]);
const byte PotCount = sizeof(PotPins) / sizeof(PotPins[0]);
Bounce buttons[ButtonCount];

/* SETUP */
void setup() {
  Serial.begin(115200);
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
  Serial.print("got lightmode ");
  Serial.print(lightMode);
  Serial.println(" from eeprom");
  if (lightMode < 0 || lightMode > 3)
    lightMode = 2;
  SDVXHID.setLightMode(lightMode);
  
  //boot animation
/*  uint16_t anim[] = {1, 4, 16, 64, 256, 128, 32, 8, 2};
  animate(anim, 9, 100);
  animate(anim, 9, 100);
  uint16_t anim2[] = {1 + 4 + 16 + 64 + 256, 2 + 8 + 32 + 128};
  animate(anim2, 2, 500);
  animate(anim2, 2, 500);*/
}

/* LOOP */
unsigned long lastReport = 0;
uint32_t prevButtonsState = 0;
  uint32_t encL = 0;
  uint32_t encR = 0;
bool modeChanged = false;
void loop() {
  /* BUTTONS */
  uint32_t buttonsState = 0;
  encL++;
  encR++;
  if (encL > 32767) encL = 0;
  if (encR > 32767) encR = 0;
  
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
      but_lights(buttonsState & 0x1ff);
      break;
    /* HID mode, only based on received HID data */
    case 1:
      SDVXHID.updateLeds(0, false);
      break;
    /* Combined inverse mode, received HID data and button state are combined then inverted */
    case 4:
      SDVXHID.updateLeds(buttonsState & 0x1ff, true);
      break;
    /* Combined mode, received HID data and button state are combined */
    case 3:
      SDVXHID.updateLeds(buttonsState & 0x1ff, false);
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
void but_lights(uint16_t lightDesc) {
  for (int i = 0; i < 9; i++) {
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
    but_lights(tab[i]);
    delay(mswait);
  }
}
