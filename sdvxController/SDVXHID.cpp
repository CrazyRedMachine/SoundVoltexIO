/* Arduino-HID-Lighting-Library
 * 
 * This Arduino-HID-Lighting-Library is derived from Arduino-HID-Lighting, whose copyriht owner is mon.
 * More information about Arduino-HID-Lighting you can find under:
 * 
 * mon's Arduino-HID-Lighting
 * https://github.com/mon/Arduino-HID-Lighting
 * 
 * 2018 (C) Arduino-HID-Lighting-Library, Knuckleslee
*/
#include "SDVXHID.h"

byte extern LightPins[];
CRGB extern left_leds[SIDE_NUM_LEDS];
CRGB extern right_leds[SIDE_NUM_LEDS];


/* HID DESCRIPTOR */
static const byte PROGMEM _hidReportSDVX[] = {
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x05,        // Usage (Game Pad)

0xA1, 0x01,        // Collection (Application)
  0x85, 0x04,        //   Report ID (4)
    /* 9 buttons (start, a b c d, fx l, fx r, service, test */
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x09,        //   Usage Maximum (0x09)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x95, 0x09,        //   Report Count (9)
  0x75, 0x01,        //   Report Size (1)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    /* 7 bits padding */
  0x95, 0x01,        //   Report Count (1)
  0x75, 0x07,        //   Report Size (7)
  0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  
    /* 2 knobs as analog axis */
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x09, 0x01,        //   Usage (Pointer)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x95, 0x02,        //   Report Count (2)
  0x75, 0x08,        //   Report Size (8)
  0xA1, 0x00,        //   Collection (Physical)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection (analog axis)

  0x85, 0x05,        //   Report ID (5)
      /* 8 button leds (no led for test button) */
    0x05, 0x09,        //     Usage Page (Buttons)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x08,        //     Usage Maximum (0x08)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x08,        //     Report Count (8)
    0x75, 0x01,        //     Report Size (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    
      /* 1 rgb controller led (with 0-255 values for red green and blue) */
    0x05, 0x08,        //     Usage Page (LEDs)
    0x19, 0x48,        //     Usage Minimum (Indicator Red)
    0x29, 0x4B,        //     Usage Maximum (Generic Indicator)
    0x15, 0x00,        //     Logical Minimum (0)
    0x26, 0xFF, 0x00,  //     Logical Maximum (255)
    0x95, 0x03,        //     Report Count (3)
    0x75, 0x08,        //     Report Size (8)
    0xA1, 0x02,        //   Collection (Logical)
      0x09, 0x48,        //       Usage (Indicator Red)
      0x09, 0x49,        //       Usage (Indicator Green)
      0x09, 0x4B,        //       Usage (Generic Indicator)
      0x91, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection (RGB led)
    
0xC0  // End Collection (Gamepad)

};

/* PluggableUSBModule IMPLEMENTATION */

    SDVXHID_::SDVXHID_(void) : PluggableUSBModule(1, 1, epType) {
      epType[0] = EP_TYPE_INTERRUPT_IN;
      PluggableUSB().plug(this);
    }

    int SDVXHID_::getInterface(byte* interfaceCount) {
      *interfaceCount += 1; // uses 1
      HIDDescriptor hidInterface = {
        D_INTERFACE(pluggedInterface, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
        D_HIDREPORT(sizeof(_hidReportSDVX)),
        D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01) // this last parameter is the bInterval (requested polling rate)
      };
      return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
    }
    
    int SDVXHID_::getDescriptor(USBSetup& setup)
    {
      // Check if this is a HID Class Descriptor request
      if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) { return 0; }
      if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) { return 0; }

      // In a HID Class Descriptor wIndex contains the interface number
      if (setup.wIndex != pluggedInterface) { return 0; }

      return USB_SendControl(TRANSFER_PGM, _hidReportSDVX, sizeof(_hidReportSDVX));
    }
    
    bool SDVXHID_::setup(USBSetup& setup)
    {
      if (pluggedInterface != setup.wIndex) {
        return false;
      }

      byte request = setup.bRequest;
      byte requestType = setup.bmRequestType;

      if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE)
      {
        return true;
      }
      
      if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
        if (request == HID_SET_REPORT) {
          if(setup.wValueH == HID_REPORT_TYPE_OUTPUT && setup.wLength == 5){
            lastHidUpdate = millis();
            USB_RecvControl(led_data, 5);
            return true;
          }
        }
      }

      return false;
    }
    
    uint8_t SDVXHID_::getShortName(char *name)
    {
      name[0] = 'S';
      name[1] = 'D';
      name[2] = 'V';
      name[3] = 'X';
      return 4;
    }

/* CUSTOM SDVX FUNCTIONS */

    void SDVXHID_::initRGB(){
      FastLED.addLeds<WS2812, A1, GRB>(left_leds, SIDE_NUM_LEDS);
      FastLED.addLeds<WS2812, A0, GRB>(right_leds, SIDE_NUM_LEDS);
      FastLED.setBrightness( 0xFF );
      delay(2000);
      CRGB color = 0;
      for (int i=0; i<SIDE_NUM_LEDS;i++){
        left_leds[i] = color;
        right_leds[i] = color;
      }
      FastLED.show();
    }
    
    void SDVXHID_::setRGB(CRGB left, CRGB right){
      for (int i=0; i<SIDE_NUM_LEDS;i++){
        left_leds[i] = left;
        right_leds[i] = right;
      }
      FastLED.show();
    }
    
    uint8_t SDVXHID_::getLightMode(){
      return lightMode;
    }
    
    unsigned long SDVXHID_::getLastHidUpdate(){
      return lastHidUpdate;
    }
    
    void SDVXHID_::setLightMode(uint8_t mode){
      if ((mode > 4) || (mode < 0)) {
        lightMode = 2;
        return;
      }
      lightMode = mode;
    }
    
    void SDVXHID_::updateLightMode(){
      uint32_t* bitfield = (uint32_t*)&(led_data[1]);
      if (*bitfield>>28&1){
        uint8_t mode = (*bitfield>>24) & 0x0F;
        setLightMode(mode); 
        *bitfield &= ~((uint32_t)0xFF<<24);
      }
    }

    void SDVXHID_::updateSideLeds(CRGB base, uint32_t encL, uint32_t encR){
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
  left_leds[i].setRGB(red/2, 0, blue/2);
  left_leds[i] += base;
  right_leds[i].setRGB(red/2, 0, blue/2);
  right_leds[i] += base;
      }
      
      FastLED.show();
    }
     
    void SDVXHID_::updateLeds(uint32_t buttonsState, uint32_t encL, uint32_t encR, bool invert){
      uint32_t* bitfield = (uint32_t*)&(led_data[1]);
      uint32_t leds = (*bitfield|buttonsState);
      if (invert)
        leds = ~leds;
      for(int i = 0; i < 7; i++) {
        if (leds>>i&1)
          digitalWrite(LightPins[i],HIGH);
        else
          digitalWrite(LightPins[i],LOW);
      }
//controller leds 
  CRGB color;
  color.setRGB(led_data[2],led_data[3],led_data[4]);
  updateSideLeds(color,encL,encR);
   
    }

    int SDVXHID_::sendState(uint32_t buttonsState, uint32_t enc1, uint32_t enc2){
      uint8_t data[5];
      data[0] = (uint8_t) 4; //report id
      data[1] = (uint8_t) (buttonsState & 0xFF);
      data[2] = (uint8_t) (buttonsState >> 8) & 0xFF;
      data[3] = (uint8_t) (enc1>>2 & 0xFF);
      data[4] = (uint8_t) (enc2>>2 & 0xFF);
      return USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, 5);
    }
