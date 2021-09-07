#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <EEPROM.h>
#include "SDVXHID.h"

SDVXHID_ SDVXHID;

#define REACTIVE_FALLBACK ((millis()-SDVXHID.getLastHidUpdate()) > 3000)
/* Buttons + Lights declarations */
//a b c d fx-l fx-r service test start (must keep this order to be compatible with konami spoof)
uint8_t ButtonPins[] = {BUT_PIN_A,BUT_PIN_B,BUT_PIN_C,BUT_PIN_D,BUT_PIN_FXL,BUT_PIN_FXR,BUT_PIN_SERVICE,BUT_PIN_TEST,BUT_PIN_START};
uint8_t LightPins[] = {LED_PIN_A,LED_PIN_B,LED_PIN_C,LED_PIN_D,LED_PIN_FXL,LED_PIN_FXR,LED_PIN_START};
uint8_t PotPins[] = {POT_PIN_L,POT_PIN_R};
CRGB left_leds[SIDE_NUM_LEDS];
CRGB right_leds[SIDE_NUM_LEDS];

const byte ButtonCount = sizeof(ButtonPins) / sizeof(ButtonPins[0]);
const byte LightCount = sizeof(LightPins) / sizeof(LightPins[0]);
const byte PotCount = sizeof(PotPins) / sizeof(PotPins[0]);
Bounce buttons[ButtonCount];

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
  
  uint8_t lightMode;
  EEPROM.get(0, lightMode);
  if (lightMode >= NUM_LIGHT_MODES)
    lightMode = 2;
  SDVXHID.setLightMode(lightMode);

  SDVXHID.initRGB();
  
  //boot animation
  SDVXHID.setRGB(CRGB::Blue, CRGB::Blue);
  uint16_t anim[] = {256, 1, 2, 4, 8, 32, 16, 0};
  animate(anim, 8, 100);
  animate(anim, 8, 100);
  SDVXHID.setRGB(CRGB::Red, CRGB::Red);
  uint16_t anim2[] = {1+2+4+8, 16+32+256};
  animate(anim2, 2, 500);
  animate(anim2, 2, 500);
  SDVXHID.setRGB(0, 0);
}

/* LOOP */
unsigned long lastReport = 0;
int32_t encL = 0;
int32_t encR = 0;

bool modeChanged = false;
void loop() {
  /* BUTTONS */
  uint32_t buttonsState = 0;

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
    SDVXHID.sendState(buttonsState, encL, encR);
    lastReport = micros();

    //check for HID-requested lightmode change
    SDVXHID.updateLightMode();
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

  /* MANUAL LIGHTMODE UPDATE (service + a) */
  if ( buttonsState & 64 ) {
    if ( (buttonsState & 1) && (modeChanged == false)) {
      modeChanged = true;
      uint8_t mode = SDVXHID.getLightMode()+1;
      if (mode == NUM_LIGHT_MODES) mode = 0;
      SDVXHID.setLightMode(mode);
      EEPROM.put(0, mode);
    }
    else if (!(buttonsState&1)) {
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
