[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/donate?hosted_button_id=WT735CX4UMZ9U)

# SoundVoltexIO

Arduino Leonardo firmware for Sound Voltex arcade panel, with native SDVX EAC compatibility

# Demo

TODO

# Acknowledgments

This basically started as a fork of my [pop'n music controller](https://github.com/CrazyRedMachine/UltimatePopnController/)

# Supported devices and requirements

This code was designed for Arduino Leonardo.

The controller code requires the Bounce2 library by Thomas O Fredericks. It can be installed from the Arduino IDE Library manager.

# Features

## I/O

In its default state, this controller has 9 buttons (7 buttons + service + test), 7 button lights, and 2 ws2812b led strips.

Because it is meant for an arcade cab panel, the knobs are potentiometers rather than encoders as it is often the case with DIY controllers.

## SDVX EAC

The controller will simulate a Sound Voltex NEMSYS controller for native compatibility with SDVX EAC.

To maintain compatibility you are forced to keep the current button ordering (buttons 1 to 6 are BT-A to BT-D, FX-L, FX-R, then Start is button 9).

Note that enabling this will hinder access to the Arduino serial interface and you'll be forced to use the reset button each time you want to reflash the firmware.

This can be disabled by setting `KONAMI_SPOOF` to 0 in SDVXHID.h

## Light modes

There are 7 different modes :

### Reactive mode

Buttons light on press, left knob makes the side leds fade to blue, right knob fade to red.

This mode is meant to be used for emulators or other games with no HID light support.

### HID mode

This is the original IO board mode where only messages from the game can control the lamps.

### Mixed mode (default mode)

This mode behaves like HID mode as soon as messages are received. If 3 seconds elapse without any received message, the firmware switches to reactive behavior (and will switch back to HID as soon as new HID messages are received).

### Combined mode

This combines the HID messages with button presses for instant lighting, **and each knob makes its respective side fade to blue while mixing with HID**. This is my favorite mode, please do try it :)

### Invert mode

This is like the Combined mode except ON and OFF states are switched around, and RGB values are complemented to 0xFF just for the fun of it.

### Rainbow mode

This is kinda like Combined mode except the knobs make rainbow patterns marching in the knob direction.

### TC mode

This is kinda like Combined mode except the knobs make blue and red patterns marching in the knob direction.

## Switching lightmodes

You can either press BT-A while holding service to switch between modes, or you can request a mode change with an HID message, which is incredibly useful in a multiboot environment as you can set the proper lightmode from the commandline before launching a game.

# Pinout

Arduino Leonardo has 5V logic therefore one can directly connect 5V leds to it.

![pinout](https://github.com/CrazyRedMachine/SoundVoltexIO/blob/main/pinout_leonardo.png?raw=true)

## Donation

If this project helps you and you want to give back, you can help me with my future projects.

While not necessary, donations are much appreciated and will only go towards funding future github projects (arcade hardware ain't cheap :( ).

Of course you'll also receive my gratitude and I'll remember you if you post a feature request ;)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate?hosted_button_id=WT735CX4UMZ9U)
