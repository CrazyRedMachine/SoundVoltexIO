# SoundVoltexIO

Arduino Leonardo firmware for Sound Voltex arcade panel

# Demo

TODO

# Acknowledgments

This basically started as a fork of my (pop'n music controller)[https://github.com/CrazyRedMachine/UltimatePopnController/]

# Supported devices and requirements

This code was designed for Arduino Leonardo.

The controller code requires the Bounce2 library by Thomas O Fredericks. It can be installed from the Arduino IDE Library manager.

# Features

## I/O

In its default state, this controller has 9 buttons (7 buttons + service + test), 7 button lights, and 2 ws2812b led strips.

It is possible to go up to 9 button lights by soldering additional headers to the Leonardo, to get access to TXLED and RXLED. It is also possible to use an Arduino Micro (not pro micro) instead.

Because it is meant for an arcade cab panel, the knobs are potentiometers rather than encoders as it is often the case with DIY controllers.

## Light modes

There are 6 different modes :

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

## Switching lightmodes

You can either press BT-A while holding service to switch between modes, or you can request a mode change with an HID message, which is incredibly useful in a multiboot environment as you can set the proper lightmode from the commandline before launching a game.

TODO: I included pre-compiled binaries and sources in the "ModeSwitch" folder. Refer to readme.md inside that folder for more details.

# Pinout

Arduino Leonardo has 5V logic therefore one can directly connect 5V leds to it.

![pinout](https://github.com/CrazyRedMachine/UltimatePopnController/blob/master/pinout_leonardo.png?raw=true)
