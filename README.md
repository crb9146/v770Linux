# v770Linux
An experimental Linux driver for controlling the Viper v770 RGB Keyboard. 

**This project is in early alpha.**

I've been using this Viper v770 RGB keyboard for years, but since going Linux full time one thing I had to sacrifice was the Windows only app for changing the key colors. Up until now I've been using VirtualBox with USB passthrough to change the lights (which isn't often anyway since you can load profiles by pressing FN + F1-F5).

But recently I decided to see if I can write my own "driver" by using Wireshark with USBPcap and reverse engineering the protocol used to change the lights. I've been able to get some results and have some control working. This works in userspace.

This keyboard is not yet supported by [OpenRGB](https://github.com/CalcProgrammer1/OpenRGB), so maybe if I get good results I can contribute this to that project.

Currently this project supports:

* Setting one of the predefined animated profiles (such as the Rainbow Wave).
* Loading one of your custom user-defined lighting profiles.
* Setting a static color on every key.

Future features to support:
* Setting complete custom profiles (EX. changing 1 key to any RGB color).
* A proper CLI and maybe a GUI.

### Requirements
`libusb1-devel` \
Fedora Install: \
``sudo dnf install libusb1-devel``

### Building
``gcc driver.c -I/usr/include/libusb-1.0 -lusb-1.0 ; sudo ./a.out``\
(This will update once the program becomes more developed.)

### Disclaimer

Although rare, sending incorrect packets to these types of devices could possibly result in it being damaged or bricked. I have not had any bricking with this keyboard, but please be aware of this when using the program. Use at your own risk (OpenRGB has a [similar warning](https://github.com/CalcProgrammer1/OpenRGB?tab=readme-ov-file#warning)).
