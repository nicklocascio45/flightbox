# FlightBox

## Background and Motivation

Over the past year I have developed a deep interest in embedded software. I have some professional IoT experience, primarily on the cloud side of things, but understanding how everything is working under the hood is what really excites me. At the beginning of the year I decided to officially jump into the embedded world and learn as much as I could about it. I quickly came to realize that it's a VAST landscape and there is an overwhelming amount to learn. I also know that simply buying a bunch of online courses is not how I learn best. I learn through building. Figuring out what to build was the question that kept me up at night for months, because I didn't know where to start.

I moved to Seattle in October, and I live directly on a high-traffic flight path for both SeaTac and Boeing Field. I see an airplane almost every minute. I also suffer from the overwhelming urge to learn where exactly each of those planes is either going to or coming from. I would open up a flight tracking app, wait 60 seconds for the advertisements to clear, and at that point the plane was gone. That's when it hit me that I had the perfect project sitting right in front of me. Why not build my own flight tracker from scratch, and throw as many embedded concepts as I could at it? So I got to work...

I've had a Raspberry Pi sitting around for years, and during the initial phases of my embedded learning journey I had acquired a handful of microcontrollers and sensors. If my goal was to just build a quick and dirty flight tracker, I could have probably just used the Pi and a low cost LCD screen, but that wasn't the goal. Over the past month I have put together the design for a three-node system that will work together to provide me all of the information I could ever need about the planes flying above me. It very intentionally incorporates many different concepts in the embedded world and many different layers of the stack. There's a bare metal STM32 node where all of the drivers are written from scratch. There's an ESP32 node running FreeRTOS and utilizing the ESP-IDF driver libraries. There's the Pi linux node serving as my own ADS-B receiver and MQTT broker. Three nodes, each with a different but critical purpose and a unique learning goal.

I'm working on finalizing the architecture diagram which I'll include in this document in the near future. This project is still in the very early stages, but I do have a rough working prototype described below. This is just the beginning, and I have a ton of ideas for where this can go. If you want to talk more about this or have any constructive feedback for me, please reach out on [my LinkedIn](https://www.linkedin.com/in/nick-locascio1/)!

## Architecture
===== Coming Soon =====

## Current Functionality (V0)
- Raspberry Pi
    - 1090 MHz ADS-B Antenna -> RTL-SDR dongle -> [ultrafeeder container](https://github.com/sdr-enthusiasts/docker-adsb-ultrafeeder) is receiving messages from planes in my area and feeding that data to several ADS-B networks. Those resources aren't in GitHub yet because I'm still playing around with it quite a bit
    - `flight-detector` Python service is currently polling an API for planes in my area, determining whether or not they should be visible, fetching more details on visible planes, and pushing those details to an MQTT topic
        - This service is going to change drastically (see roadmap), but this was my fastest route to getting something up and running so that I could focus on more of the embedded work
    - mosquitto MQTT broker up and running
- ESP32
    - Working WiFi connection
    - MQTT client connecting to broker, listening on set topic, and parsing JSON payloads that it receives
    - Lighting component in its own FreeRTOS task that waits for an event bit to get set and blinks a certain LED based on the set bit
- STM32
    - Just a very basic build process working with Makefile, startup code, and linker script
    - GPIO code to blink the onboard LED
    - UART code to print to serial monitor and read characters from serial monitor
    - All very simple but for my first bare metal experience, this took quite a bit of effort to get working correctly

## Roadmap

### V1
This is what I envision the first true iteration of this project looking like:
- Move flight detection off of third-party API and just use my ADS-B data directly, with very fine-tuned bounding boxes defining which planes should be "visible"
- ESP32 is connected to an LCD screen and displays interesting details about planes that are visible to me from my couch
- ESP32 has some buttons to provide feedback on system performance (missed detection, inaccurate detection, etc.)
- STM32 is connected to two presence sensors to determine if I am actually sitting on the couch or not
- STM32 is also connected to an environmental sensor to read temperature, pressure, and humidity
- The project code for each component is cleaned up significantly and more "production-ready"
- Some automation in place for deployment to each component (probably just in the form of better Makefiles and scripts)

### V2
- STM32 has its own mini display for presence + environmental conditions
- STM32 is transmitting its data to the ESP32 over a CAN bus
- ESP32 is sending STM32's data to the Pi via MQTT
- Pi is factoring the STM32 data into its detection/decision making process
- OTA updates for at least ESP32 and ideally STM32 as well

### V3
- Python service is rewritten in Rust (purely for the sake of learning Rust, not because it's necessary)
- More to come

### Stretch/Longer Term Items
- I think it would be extremely cool to bake a bunch of this into a handheld device. I've been researching what it would take and I know it's some combination of position triangulation, bluetooth, PCB design, and antenna design. It's just an idea I had one night that I want to dive into more, but this could be a whole new project
- I've also learned that you can capture satellite data with a certain antenna setup. It would be cool to dive into that as well
