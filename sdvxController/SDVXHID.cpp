#include "SDVXHID.h"

byte extern LightPins[];
CRGB extern left_leds[SIDE_NUM_LEDS];
CRGB extern right_leds[SIDE_NUM_LEDS];

/* HID DESCRIPTOR */
static const byte PROGMEM _hidReportSDVX[] = {
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x04,        // Usage (Joystick)

0xA1, 0x01,        // Collection (Application)
  0x85, 0x04,        //   Report ID (4)
    /* 9 buttons (a b c d, fx l, fx r, service, test, start */
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

   /*Lights */
    0x85, 0x05,                    /*   REPORT_ID 5*/ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x25, 0x01,                    /*     LOGICAL_MAXIMUM (1) */ 
    /*Led A */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x01,                    /*     USAGE (Instance 1) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x04,                    /*       STRING INDEX (4) */
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led B */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x02,                    /*     USAGE (Instance 2) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x05,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led C */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x03,                    /*     USAGE (Instance 3) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x06,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led D */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x04,                    /*     USAGE (Instance 4) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x07,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led FX-L */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x05,                    /*     USAGE (Instance 5) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x08,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led FX-R */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x06,                    /*     USAGE (Instance 6) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x09,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led Start */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x07,                    /*     USAGE (Instance 7) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0a,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*  Reserved 1 bits */ 
    0x95, 0x01,            /*   REPORT_COUNT (1) */ 
    0x75, 0x01,            /*   REPORT_SIZE (7) */ 
    0x91, 0x03,            /*   OUTPUT (Cnst,Var,Abs) */ 

    /* 1 rgb controller led (with 0-255 values for red green and blue) */
    /* Red */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x08,                    /*     USAGE (Instance 7) */ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x26, 0xFF, 0x00,              /*     LOGICAL_MAXIMUM (1) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0b,
    0x75, 0x08,                    /*       REPORT_SIZE (8) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /* Green */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x09,                    /*     USAGE (Instance 7) */ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x26, 0xFF, 0x00,              /*     LOGICAL_MAXIMUM (1) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0c,
    0x75, 0x08,                    /*       REPORT_SIZE (8) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /* Blue */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x0a,                    /*     USAGE (Instance 7) */ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x26, 0xFF, 0x00,              /*     LOGICAL_MAXIMUM (1) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0d,
    0x75, 0x08,                    /*       REPORT_SIZE (8) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
   
    /* HID modeswitch request */
    0x85, 0x07,                    /*   REPORT_ID 7 */
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x19, 0x00,                    /*     USAGE_MINIMUM (Button 1) */ 
    0x29, 0x04,                    /*     USAGE_MAXIMUM (Button 4)*/ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x25, 0x04,                    /*     LOGICAL_MAXIMUM (4) */
    0x95, 0x01,                    /*     REPORT_COUNT (1) */ 
    0x75, 0x04,                    /*     REPORT_SIZE (4) */ 
    0xb1, 0x02,                     /*    FEATURE (Data,Var,Abs) */ 
    /* Reserved bits */ 
    0x95, 0x01,                      /*   REPORT_COUNT (1) */ 
    0x75, 0x04,                      /*   REPORT_SIZE (4) */ 
    0xb1, 0x03,                      /*   FEATURE (Cnst,Var,Abs) */
    
0xC0  // End Collection (Joystick)

};

/* PluggableUSBModule IMPLEMENTATION */

#if KONAMI_SPOOF == 1
const DeviceDescriptor PROGMEM USB_DeviceDescriptorIAD =
  D_DEVICE(0xEF,0x02,0x01,64,0x1ccf,0x101c,0x100,IMANUFACTURER,IPRODUCT,ISERIAL,1);
const char* const PROGMEM String_Manufacturer = "Konami Amusement";
const char* const PROGMEM String_Product = "SOUND VOLTEX controller";
#else
/* HID descriptor strings */
const char* const PROGMEM String_Manufacturer = "CrazyRedMachine";
const char* const PROGMEM String_Product = "SDVX Controller";
#endif
const char* const PROGMEM String_Serial = "SDVX";

const char* const PROGMEM LEDString_00 = "BT-A";
const char* const PROGMEM LEDString_01 = "BT-B";
const char* const PROGMEM LEDString_02 = "BT-C";
const char* const PROGMEM LEDString_03 = "BT-D";
const char* const PROGMEM LEDString_04 = "FX-L";
const char* const PROGMEM LEDString_05 = "FX-R";
const char* const PROGMEM LEDString_06 = "Start";
const char* const PROGMEM LEDString_07 = "Controller R";
const char* const PROGMEM LEDString_08 = "Controller G";
const char* const PROGMEM LEDString_09 = "Controller B";

const char* String_indiv[] = {LEDString_00,LEDString_01,LEDString_02,LEDString_03,LEDString_04,LEDString_05,LEDString_06,LEDString_07,LEDString_08,LEDString_09};
uint8_t STRING_ID_Count = 10;

static bool SendControl(uint8_t d)
{
  return USB_SendControl(0, &d, 1) == 1;
}

static bool USB_SendStringDescriptor(const char *string_P, uint8_t string_len, uint8_t flags) {
        SendControl(2 + string_len * 2);
        SendControl(3);
        bool pgm = flags & TRANSFER_PGM;
        for(uint8_t i = 0; i < string_len; i++) {
                bool r = SendControl(pgm ? pgm_read_byte(&string_P[i]) : string_P[i]);
                r &= SendControl(0); // high byte
                if(!r) {
                        return false;
                }
        }
        return true;
}

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
      #if KONAMI_SPOOF == 1  
    if(setup.wValueH == USB_DEVICE_DESCRIPTOR_TYPE) {
        return USB_SendControl(TRANSFER_PGM, (const uint8_t*)&USB_DeviceDescriptorIAD, sizeof(USB_DeviceDescriptorIAD));
    }
#endif
  if (setup.wValueH == USB_STRING_DESCRIPTOR_TYPE) { 
      if (setup.wValueL == IPRODUCT) {
          return USB_SendStringDescriptor(String_Product, strlen(String_Product), 0);
      } 
      else if (setup.wValueL == IMANUFACTURER) {
          return USB_SendStringDescriptor(String_Manufacturer, strlen(String_Manufacturer), 0);
      }        
      else if (setup.wValueL == ISERIAL) {
          return USB_SendStringDescriptor(String_Serial, strlen(String_Serial), 0);
      }
      else if(setup.wValueL >= STRING_ID_Base && setup.wValueL < (STRING_ID_Base + STRING_ID_Count)) {
          return USB_SendStringDescriptor(String_indiv[setup.wValueL - STRING_ID_Base], strlen(String_indiv[setup.wValueL - STRING_ID_Base]), 0);
      }                       
  }
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
        /* lightmode info */
        if (setup.wValueH == HID_REPORT_TYPE_FEATURE && setup.wValueL == 7) {
          uint8_t lm_data[2] = {7,0x00};
          lm_data[1] = lightMode;
          USB_SendControl(0, &lm_data, 2);
          return true;
        }
        
        return true;
      }
      
      if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
        if (request == HID_SET_REPORT) {
          if(setup.wValueH == HID_REPORT_TYPE_OUTPUT && setup.wLength == 5){
            lastHidUpdate = millis();
            USB_RecvControl(led_data, 5);
            return true;
          }
          else if (setup.wValueH == HID_REPORT_TYPE_FEATURE ) {
          /* lightmode switch */
            byte usb_data[5];
            USB_RecvControl(usb_data, setup.wLength);
            if (usb_data[0] == 7)
            {
              mode_data = usb_data[1];
              return true;
            }
          }
        }
      }

      return false;
    }

/* CUSTOM SDVX FUNCTIONS */

    void SDVXHID_::initRGB(){
      FastLED.addLeds<WS2812, RGB_PIN_L, GRB>(left_leds, SIDE_NUM_LEDS);
      FastLED.addLeds<WS2812, RGB_PIN_R, GRB>(right_leds, SIDE_NUM_LEDS);
      FastLED.setBrightness( 0xFF );
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
    
    uint8_t SDVXHID_::setLightMode(uint8_t mode){
      uint8_t effective_mode = mode;
      if ((effective_mode >= NUM_LIGHT_MODES)) {
        effective_mode = 2;
      }
      lightMode = effective_mode;
      mode_data = lightMode;
      return lightMode;
    }
    
    void SDVXHID_::updateLightMode(){
      uint8_t mode = mode_data;
        if (mode != lightMode) {
          mode_data = setLightMode(mode);
        }
     }

#define FADE_RATE 512
void SDVXHID_::update_knobs_param(){
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
    } else {
      if (red > 0)
        red--;
    }

    /* compute spinL/spinR (which are spinEncL/R but with a cooldown on the 0 so there's inertia in rainbow/tc */
    if (spinEncL != 0)
      knobs_param.spinL = spinEncL;
    if (spinEncR != 0)
      knobs_param.spinR = spinEncR;
    if (red == 0)
      knobs_param.spinR = 0;
    if (blue == 0)
      knobs_param.spinL = 0;
                
    /* blue/red factor is a [0;1] value representing the ratio of blue/red shift
    (0 means no change with respect to base color, 1 means full blue/red instead */
    knobs_param.blueFactor = ((float)blue/(float)FADE_RATE);
    knobs_param.redFactor = ((float)red/(float)FADE_RATE);
    /* brightness is just max between blueFactor or redFactor, scaled to 0xFF */
    knobs_param.brightness = 255*(knobs_param.blueFactor > knobs_param.redFactor ? knobs_param.blueFactor:knobs_param.redFactor);  
}

    /**
     * update controller led with HID base request + a color shifted value depending on knobs activity
     */
    void SDVXHID_::updateSideLeds(CRGB base, bool invert, bool knobs, bool hid){
      static uint16_t blue = 0;
      static uint16_t red = 0;

      if (invert)
      {
        base.r = 0xFF - base.r;
        base.g = 0xFF - base.g;
        base.b = 0xFF - base.b;
      }
      if (!knobs)
      {
        setRGB(base,base);
        return;      
      }
      
      /* Update blue/red shift amount according to knob motion */
      update_knobs_param();

      /* apply light */
      if (!hid){
        CRGB left,right;
        left.setRGB(255*knobs_param.redFactor/2, 0, 255*knobs_param.blueFactor);
        right.setRGB(255*knobs_param.redFactor, 0, 255*knobs_param.blueFactor/2);
        setRGB(left,right);
        return;
      }
      /* HID color shift */
      float redL, greenL, blueL, redR, greenR, blueR;

      if (invert)
      {
      redL = knobs_param.blueFactor*base.g + knobs_param.blueFactor*base.b + base.r;
      greenL = knobs_param.blueFactor*base.r + knobs_param.blueFactor*base.b + base.g;
      blueL = (1-knobs_param.blueFactor)*base.b;
      if (redL > 255) redL = 255;
      if (greenL > 255) greenL = 255;
      
      redR = (1-knobs_param.redFactor)*base.r;
      greenR = knobs_param.redFactor*base.r + knobs_param.redFactor*base.b + base.g;
      if (greenR > 255) greenR = 255;
      blueR = base.b + knobs_param.redFactor*base.r + knobs_param.redFactor*base.g; 
      if (blueR > 255) blueR = 255;
      
      } else {
        
      redL = (1-knobs_param.blueFactor)*base.r;
      greenL = (1-knobs_param.blueFactor)*base.g;
      blueL = knobs_param.blueFactor*base.r + knobs_param.blueFactor*base.g + base.b;
      if (blueL > 255) blueL = 255;
      
      redR = base.r + knobs_param.redFactor*base.g + knobs_param.redFactor*base.b;
      if (redR > 255) redR = 255;
      greenR = (1-knobs_param.redFactor)*base.g;
      blueR = (1-knobs_param.redFactor)*base.b;
      }
      
      CRGB rgbL, rgbR;
      rgbL.setRGB(redL, greenL, blueL);
      rgbR.setRGB(redR, greenR, blueR);
      setRGB(rgbL,rgbR);
    }

    static void SDVXHID_::write_button_leds(uint32_t buttonsState, bool invert, bool hid)
    {
      uint32_t bitfield = led_data[1];
      //fix start button led position
      if (bitfield & 0x40)
      {
        bitfield |= 0x100;
      }
      
      uint32_t leds = buttonsState;
      if (hid)
        leds |= bitfield;
        
      if (invert)
        leds = ~leds;

      for(int i = 0; i < NUM_BUT_LEDS-1; i++) {
        if (leds>>i&1)
          digitalWrite(LightPins[i],HIGH);
        else
          digitalWrite(LightPins[i],LOW);
      }
      //exception: start is button 9
       if (leds>>8&1)
          digitalWrite(LightPins[6],HIGH);
        else
          digitalWrite(LightPins[6],LOW); 
    }
    
    void SDVXHID_::updateLeds(uint32_t buttonsState, bool invert, bool knobs, bool hid){
      
      write_button_leds(buttonsState, invert, hid);
      /* side leds */
      if (knobs || hid)
      {
        CRGB color;
        color.setRGB(led_data[2],led_data[3],led_data[4]);      
        updateSideLeds(color,invert,knobs,hid);
      }
    }

    void SDVXHID_::rainbowLeds(uint32_t buttonsState){
      
      write_button_leds(buttonsState, false, true);

      /* side leds */    
      update_knobs_param();

      uint8_t thisHue = beat8(50,255);
        
      if (knobs_param.brightness == 0)
      {
        CRGB color;
        color.setRGB(led_data[2],led_data[3],led_data[4]);
        setRGB(color, color);
      }
      else
      {
        FastLED.setBrightness(knobs_param.brightness);

        if (knobs_param.blueFactor != 0)
        {
          if (knobs_param.spinL) fill_rainbow(left_leds, SIDE_NUM_LEDS-1, knobs_param.spinL*thisHue, 15);
          else fill_solid(left_leds, SIDE_NUM_LEDS, CRGB::Blue);
        } 
        else 
        {
          CRGB color;
          color.setRGB(led_data[2],led_data[3],led_data[4]);
          fill_solid(left_leds, SIDE_NUM_LEDS, color);
        }
        if (knobs_param.redFactor != 0)
        {
          if (knobs_param.spinR) fill_rainbow(right_leds, SIDE_NUM_LEDS-1, knobs_param.spinR*thisHue, 15); 
          else fill_solid(right_leds, SIDE_NUM_LEDS, CRGB::Red);
        } 
        else 
        {
          CRGB color;
          color.setRGB(led_data[2],led_data[3],led_data[4]);
          fill_solid(right_leds, SIDE_NUM_LEDS, color);
        }
      }
      FastLED.show();
      FastLED.setBrightness(0xFF);
    }

    static void fill_tc( struct CRGB * pFirstLEDL, struct CRGB * pFirstLEDR, bool hasBlue,
                  int8_t spinL, bool hasRed,
                  int8_t spinR, CRGB hidcolor )
    {
      uint8_t thisHue = beat8(50,255); 
      uint8_t initialhueB = spinL*thisHue;
      uint8_t initialhueR = spinR*thisHue;
      int posB = (initialhueB* (2*SIDE_NUM_LEDS-4) / 255);
      int posR = (initialhueR* (2*SIDE_NUM_LEDS-4) / 255);
      
      for( int i = 0; i < SIDE_NUM_LEDS; i++) {
        pFirstLEDL[i] = CRGB::Black;
        pFirstLEDR[i] = CRGB::Black;
      } 
      pFirstLEDL[0] = hidcolor;
      pFirstLEDR[0] = hidcolor;

      DEBUG_VAR(hasBlue);
      DEBUG_VAR(hasRed);
      
      if (hasBlue&&posB>0){
        CRGB rgb;
        rgb.b = 0xFF;
        rgb.g = 0;
        rgb.r = 0;
        
        if (posB <= SIDE_NUM_LEDS-2)
        {
          pFirstLEDL[(SIDE_NUM_LEDS-1)-posB] += rgb;
          /*if (spinL == -1) {
            CRGB fade;
            fade.b = 0; //(float)posB * 50./(SIDE_NUM_LEDS/2);
            fade.r = 0;
            fade.g = 0;
            pFirstLEDR[1] += fade;
          }*/
        }
        else 
        {
          pFirstLEDR[posB-(SIDE_NUM_LEDS-2)] += rgb;
          /*if (spinL == 1) {
            CRGB fade;
            fade.r = 0;
            fade.g = 0;
            fade.b = 0; //(float)((2*SIDE_NUM_LEDS-4)-posB) * 50./(SIDE_NUM_LEDS/2);
            pFirstLEDL[1] += fade;
          }*/
        }
      }
      
      if (hasRed&&posR>0){
        CRGB rgb;
        rgb.r = 0xFF;
        rgb.g = 0;
        rgb.b = 0;
        if (posR <= SIDE_NUM_LEDS-2)
        {
          pFirstLEDL[(SIDE_NUM_LEDS-1)-posR] += rgb;
          /*if (spinR == -1) {
            CRGB fader;
            fader.g = 0;
            fader.b = 0;
            fader.r = 0; //(float)posR * 50./(SIDE_NUM_LEDS/2);
            pFirstLEDR[1] += fader;
          }*/
        }
        else 
        {
          pFirstLEDR[posR-(SIDE_NUM_LEDS-2)] += rgb;
          /*if (spinR == 1) {
            CRGB fader;
            fader.g = 0;
            fader.b = 0;
            fader.r = 0; //(float)((2*SIDE_NUM_LEDS-4)-posR) * 50./(SIDE_NUM_LEDS/2);
            pFirstLEDL[1] += fader;
          }*/
        }
      }
      
    }

    void SDVXHID_::tcLeds(uint32_t buttonsState){
      
      write_button_leds(buttonsState, false, true);
      
      /* side leds */
            
      update_knobs_param();                 
        
      CRGB color;
      color.setRGB(led_data[2],led_data[3],led_data[4]);
      if (knobs_param.brightness == 0)
      {
        setRGB(color, color);
      }
      else
      {
        FastLED.setBrightness(knobs_param.brightness);
        fill_tc(left_leds, right_leds, (knobs_param.blueFactor != 0), knobs_param.spinL, (knobs_param.redFactor != 0), knobs_param.spinR, color);     
      }
      FastLED.show();  
      FastLED.setBrightness(0xFF);
    }
    
    int SDVXHID_::sendState(uint32_t buttonsState, int32_t enc1, int32_t enc2){
      /* filtering small fluctuations (fix crazy stuttering cursor during attract mode loop) */
      static int32_t prev_enc1 = 0; 
      static int32_t prev_enc2 = 0;
      int32_t delta1 = enc1 - prev_enc1;
      int32_t delta2 = enc2 - prev_enc2;
      static byte idle_counter_L = 0;
      static byte idle_counter_R = 0;

      /* send same value as previous report if it's only small change */
      if (delta1 >= -15 && delta1 <= 15)
        enc1 = prev_enc1;
      if (delta2 >= -15 && delta2 <= 15)
        enc2 = prev_enc2;

      /* now enc1&enc2 are filtered this is also a good moment to update the spin values ( -1;0;1 ) */
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

      /* finally, send HID report */
      uint8_t data[5];
      data[0] = (uint8_t) 4; //report id
      data[1] = (uint8_t) (buttonsState & 0xFF);
      data[2] = (uint8_t) (buttonsState >> 8) & 0xFF;
      data[3] = (uint8_t) (enc1>>2 & 0xFF);
      data[4] = (uint8_t) (enc2>>2 & 0xFF);
      return USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, 5);
    }
