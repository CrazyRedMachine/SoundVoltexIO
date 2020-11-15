#include "HID.h"
#include <FastLED.h>

#define SIDE_NUM_LEDS    9
#define NUM_BUT_LEDS     7         
#define EPTYPE_DESCRIPTOR_SIZE    uint8_t

class SDVXHID_ : public PluggableUSBModule {

  public:
    SDVXHID_(void);

    void initRGB();

    void setRGB(CRGB left, CRGB right);

    void rainbowLeds(uint32_t buttonsState);

    void tcLeds(uint32_t buttonsState, int32_t encL, int32_t encR);
    
    void updateSideLeds(CRGB base, bool invert, bool knobs, bool hid);
    /**
     * Updates the led status based on led_data (HID report received) and/or button states
     * param[in] buttonState bitfield with currently pressed buttons (used to force additional lights for mixed mode)
     * param[in] invert set to true to invert on/off status (used for invert lightmode)
     * param[in] knobs set to true to use knob activity for color shift
     * param[in] hid set to true to use hid led_data 
     */
    void updateLeds(uint32_t buttonsState, bool invert, bool knobs, bool hid);

    /**
     * Sends the gamepad button states to the PC as an HID report
     * param[in] buttonsState bitfield with currently pressed buttons
     * return USB_Send() return value
     */
    int sendState(uint32_t buttonsState, int32_t enc1, int32_t enc2);

    /**
     * Changes the lightMode if a received HID report asks for it
     */
    void updateLightMode();
    
    /**
     * getter and setter for lightMode protected field.
     */
    uint8_t getLightMode();
    void setLightMode(uint8_t mode);
    
    /**
     * getter for lastHidUpdate protected field.
     */
    unsigned long getLastHidUpdate();
    
  protected:
    /* knob spin filtered direction (since idling makes the analogread go +/- 2 sometimes)
       -1 (CCW spin) 0 (idle) +1 (CW spin) */
    int8_t spinEncL = 0;
    int8_t spinEncR = 0;
    /* current lightMode (0 = reactive, 1 = HID only, 2 = mixed (HID+reactive auto-switch), 3 = combined (HID+button presses), 4 = combined invert) */
    uint8_t lightMode = 2;
    /* timestamp of last received HID report for lightMode 3 */
    unsigned long lastHidUpdate = 0;
    /* byte array to receive HID reports from the PC */
    byte led_data[6];
    byte mode_data[2];
    
    /* Implementation of the PUSBListNode */
    EPTYPE_DESCRIPTOR_SIZE epType[1];
    uint8_t protocol;
    uint8_t idle;    
    int getInterface(uint8_t* interfaceCount);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);
    uint8_t getShortName(char *name);
};

extern SDVXHID_ SDVXHID;
