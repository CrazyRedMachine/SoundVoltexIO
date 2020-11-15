# ModeSwitch for SoundVoltexIO

This is a binary meant to set the LightMode used by the SDVX Controller. It is mostly useful for multi-boot frontends as it allows to switch to the correct mode before launching a game.

For ease of use I included pre-compiled binaries. It's easier to modify the ahk version to suit your needs but I'm still including a C version as with Pop'n, in case you have trouble running the ahk version on a real cab or something.

## Usage

```modeswitch.exe <num>```

Where ``<num>`` can take the value

0 : Reactive mode (Reactive buttons + Fade to red/blue with left and right knobs)

1 : HID-only (original AC mode)

2 : Mixed (HID-only when messages come, fallback to reactive when HID messages stop coming)

3 : Combined (HID + button presses, and side color slowly shifting to red or blue when the knobs are turned)

4 : Invert (Like Combined except ON is OFF and vice-versa, also the side lights are of opposite colors)

5 : Rainbow mode (like Combined except the knobs create rainbow march patterns instead of colorshift)

6 : TC mode (like Combined except the knobs create red/blue march patterns instead of colorshift)

## How it works

On the SoundVoltex Controller firmware, there is a secondary HID output report with ID 6, meant to trigger a modeswitch.

This binary opens the USB device whose path is written in ```devicepath.dat``` and sends the correct HID report.

The autohotkey version finds the path automatically based on VID=2341 and PID=8036.

## How to retrieve devicepath (for the C version)

The included devicepath.dat should work with both Due and Leonardo versions out of the box, but in case you get the message ``Couldn't open device. Make sure devicepath.dat contains the correct path.`` and you are sure the device is plugged in, you can either go into windows device manager and look for the device path in the advanced properties of your arduino device, or the easiest way is to look at the value inside spicetools or bemanitools configuration files.

devicepath.dat file should contain the device path on its own on a single line. Multiple paths on multiple lines should work too in which case it will open the first valid path it finds.

If no valid path is found or if there is no `devicepath.dat` file, it will attempt to open the default path from the Leonardo firmware (same as the one found in this included devicepath file).
 
### spicetools

open ```%appdata%/spicetools.xml```, look for a "devid" value starting with ```\\?\HID#VID_2341&amp;PID_8036``` (Leonardo)

Note that you'll have to replace "```&amp;```" occurrences by "```&```".

### bemanitools

open ```%appdata%/DJHACKERS/sdvx_v5_00a07``` and look for a string starting with ```\\?\HID#VID_2341&PID_8036``` (Leonardo)
