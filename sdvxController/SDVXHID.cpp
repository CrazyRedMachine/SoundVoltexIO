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
      for (int i=0; i<SIDE_NUM_LEDS;i++){
        left_leds[i] = CRGB::Black;
        right_leds[i] = CRGB::Black;
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

#define FADE_RATE 512
    /**
     * update controller led with HID base request + a color shifted value depending on knobs activity
     */
    void SDVXHID_::updateSideLeds(CRGB base, int32_t encL, int32_t encR, bool hid){
      static uint16_t blue = 0;
      static uint16_t red = 0;
      /* Update blue/red shift amount according to knob motion */

      //left knob
      if (spinEncL != 0){
        if (blue<FADE_RATE)
          blue++;
      } else {
        if (blue > 0)
          blue--;
      }

      //right knob
      if (spinEncR != 0){
        if (red<FADE_RATE)
          red++;
        else red = FADE_RATE;
      } else {
        if (red > 0)
          red--;
      }
      
      /* blue/red factor is a [0;1] value representing the ratio of blue/red shift
      (0 means no change with respect to base color, 1 means full blue/red instead */
      float blueFactor = ((float)blue/(float)FADE_RATE);
      float redFactor = ((float)red/(float)FADE_RATE);
      
      /* apply light */
      if (!hid){
        CRGB left,right;
        left.setRGB(255*redFactor/2, 0, 255*blueFactor);
        right.setRGB(255*redFactor, 0, 255*blueFactor/2);
        setRGB(left,right);
        return;
      }
      /* HID color shift */
      float redL, greenL, blueL, redR, greenR, blueR;
      
      redL = (1-blueFactor)*base.r;
      greenL = (1-blueFactor)*base.g;
      blueL = blueFactor*base.r + blueFactor*base.g + base.b;
      if (blueL > 255) blueL = 255;
      
      redR = base.r + redFactor*base.g + redFactor*base.b;
      if (redR > 255) redR = 255;
      greenR = (1-redFactor)*base.g;
      blueR = (1-redFactor)*base.b;

      CRGB rgbL, rgbR;
      rgbL.setRGB(redL, greenL, blueL);
      rgbR.setRGB(redR, greenR, blueR);
      setRGB(rgbL,rgbR);
    }
     
    void SDVXHID_::updateLeds(uint32_t buttonsState, int32_t encL, int32_t encR, bool invert, bool hid){
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

      /* side leds */
      if (encL != 0 || encR != 0)
      {
        CRGB color;
        color.setRGB(led_data[2],led_data[3],led_data[4]);
        updateSideLeds(color,encL,encR,hid);
      }
    }

    int SDVXHID_::sendState(uint32_t buttonsState, int32_t enc1, int32_t enc2){
      /* filtering small fluctuations (fix crazy stuttering cursor during attract mode loop) */
      static int32_t prev_enc1 = 0; 
      static int32_t prev_enc2 = 0;
      int32_t delta1 = enc1 - prev_enc1;
      int32_t delta2 = enc2 - prev_enc2;
      static byte idle_counter_L = 0;
      static byte idle_counter_R = 0;
      
      if (delta1 >= -15 && delta1 <= 15)
        enc1 = prev_enc1;
      if (delta2 >= -15 && delta2 <= 15)
        enc2 = prev_enc2;

      /* update knob spin values */
      if (enc1 == prev_enc1){
        idle_counter_L++;
        if (idle_counter_L == 100)
        {
          spinEncL = 0;
          idle_counter_L = 0;
        }
      }
      else if (enc1 > prev_enc1 || prev_enc1 - enc1 > 1000 ) spinEncL = 1;
      else spinEncL = -1;
      
      if (enc2 == prev_enc2){
        idle_counter_R++;
        if (idle_counter_R == 100)
        {
          spinEncR = 0;
          idle_counter_R = 0;
        }
      }
      else if (enc2 > prev_enc2 || prev_enc2 - enc2 > 1000 ) spinEncR = 1;
      else spinEncR = -1;
      
      prev_enc1 = enc1;
      prev_enc2 = enc2;

      /* send HID report */
      uint8_t data[5];
      data[0] = (uint8_t) 4; //report id
      data[1] = (uint8_t) (buttonsState & 0xFF);
      data[2] = (uint8_t) (buttonsState >> 8) & 0xFF;
      data[3] = (uint8_t) (enc1>>2 & 0xFF);
      data[4] = (uint8_t) (enc2>>2 & 0xFF);
      return USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, 5);
    }
