# DK-PixelSun
ESP32 based pixel mapped LED array controlled by DMX over ArtNET

_PixelSun_ is an array of 200 individually addressable LEDs remotely controlled by [DMX](https://en.wikipedia.org/wiki/DMX512) data sent over [ArtNET](https://art-net.org.uk) wirelessly. The physical layout of the LEDs are mapped by special pixel mapping software that can play colorful video animations across the LEDs, like a giant TV screen.

![PixelSun Room Lights](/images/pixelsun_room.jpg)

I actually didn't have to do much coding on this project. The bulk of the work was all research, configuration, and setup. It's amazing how much technology already exists out there - sometimes you just need to connect the pieces and wire it up.

![PixelSun Room Lights](/images/pixelsun_lines.jpg) ![PixelSun Software](/images/pixelsun_lines_ledlab.png)

## Goal
I've explored [bitmap](https://github.com/dkrue/DK-SpectraBitmap), [audio-sensitive](https://github.com/dkrue/DK-SpectraBloom), and [procedurally](https://github.com/dkrue/DK-ImpactWings) animated LED displays in past projects. I wanted to look into the concept of [pixel mapping](https://www.enttec.com/product/controls/addressable-led-pixel-control/pixel-mapping-software/) as a means of animating a large LED display.  This gave me both a software and hardware decision to make, with two subgoals:

- Hardware: Make a project able to receive real-time [DMX](https://en.wikipedia.org/wiki/DMX512) serial communication, similar to my [ImpactWings](https://github.com/dkrue/DK-ImpactWings) project. This time around however, I wanted to receive that data over a local network from my computer.
- Software: Find the best inexpensive pixel mapping software capable of sending out [ArtNET](https://art-net.org.uk) animation data. 

![PixelSun](/images/pixelsun_soft1.jpg)

## Solution
- Hardware: Using an [Espressif ESP32](https://www.espressif.com/en/products/socs/esp32) microcontroller would let it all work over WiFi! These are increasingly popular Arduino-compatible chips that are great for networked projects like my remote-controlled [DK-Rover](https://github.com/dkrue/DK-Rover).
- Software: I found this excellent [Sparkfun article](https://learn.sparkfun.com/tutorials/using-artnet-dmx-and-the-esp32-to-drive-pixels/introduction) on how to send ArtNET data to an ESP32 and LED strips using Resolume. That's great but Resolume is expensive professional software and I needed something simple. I looked into Lightjams, Enttec Elm, xLights, and ended up going with an app called [LED Lab](https://apps.apple.com/us/app/l-e-d-lab/id832042156) because of its powerful simplicity, and it could run on my Mac.

The final piece was the addressable LEDs themselves. I often work with WS2811 / Neopixel LED strips and found these really cool diffused [LED "fairy lights" at Adafruit](https://www.adafruit.com/product/4917) that are WS2811 compatible with a few quirks.

![PixelSun](/images/pixelsun_soft2.jpg)

## Development
All of this uses my favorite LED library for the Arduino, [FastLED](https://github.com/FastLED/FastLED/wiki/Overview). I was able to use existing code from the [ArtnetWifi](https://github.com/rstephan/ArtnetWifi) library, which builds on FastLED and the ESP32's WiFi capabilities.

### Data Flow
1. The ESP32 connects to the WiFi network and displays its IP address on an onboard LCD.
2. LED Lab streams DMX data over the network to the ESP32 at about 30 frames per second.
3. The ESP32 outputs the color data to 11 LED strands using FastLED and the WS2811 protocol.

![PixelSun in Development](/images/pixelsun_development.jpg)

### Testing
With so many hand wired connections and power requirements exceeding what a USB port can provide to the ESP32, testing was really important for this project. My final goal was to suspend the entire thing from the ceiling, so everything needed to be modular for installation. I was happy to find the ESP32 logic running at 3.3 volts could power the 5 volt LEDs without any visual artifacts and would not require a logic level shifter.

Contrary to what [Adafruit](https://www.adafruit.com/product/4917) says, the middle wire of each LED strand is _not_ the data line, it is ground.

### Final Assembly
Each LED strand is soldered to a JST PH connector and plugged into 5 volt power on the final circuit for even power distribution. The entire project uses about 600mA of current, so an external DC power supply is used.

![PixelSun Control Circuit](/images/pixelsun_final.jpg)
_PixelSun_ final installation: mounted to the ceiling with 11 LED strands extending out from the central light fixture to the edges of the room.

### Pixel Mapping
The position of each LED strip is mapped in 2D space using [LED Lab's](https://apps.apple.com/us/app/l-e-d-lab/id832042156) advanced mapping tool. Now colorful shapes and images can be projected across the entire LED installation!

![PixelSun LED Mapping](/images/pixelsun_map.png)

Note that I didn't have to run any wires between strand ends, as these quirky LEDs have permanently set addresses. That means the data is able to pushed out simultaneously in parallel from a single data pin on the ESP32!

![PixelSun](/images/pixelsun_soft3.jpg) ![PixelSun with LED Lab](/images/pixelsun_soft3_ledlab.png)


