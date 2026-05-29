# FlightBox

## Background

I moved to Seattle in October, and I live right on a high-traffic flight path for both SeaTac and Boeing Field. Unfortunately, I also suffer from the need to know where any plane that I see is going or coming from. However, the flight tracking apps take a lot of time to load and are literred with ads every 30 seconds, so it gets pretty tedious.

I was scrolling Reddit one day and saw this [awesome idea](https://www.reddit.com/r/RASPBERRY_PI_PROJECTS/comments/1sbn330/flighttrackr_alerts_me_whenever_a_plane_flies/) to build a little IoT device with a Raspberry Pi that displays flight info. The idea was exactly what I was looking for, but I wanted the implementation to be all mine. I didn't look at any of the code or the details in the post, I just got to work.

For some additonal context, I have a deep interest in embedded programming, firmware, and IoT. My goal is to land a job in this domain, and I spend most of my free time working towards that goal. I am already a full-time software engineer so I'm on the right track, I just haven't worked much in this domain. I have put together a roadmap (link to github) of bare metal programming and driver development to help get me there, but thought that this project would be a fun way to integrate some of those concepts into a domain I am interested in.

That context should explain the design decisions and technology choices that I made here. I wasn't going for the most optimal implementation, I was looking to combine a bunch of the different things that I am currently trying to learn along with my professional experience all into this project.

Lastly, not a single line of the code in this project was written by AI. I feel like that is important to call out in this day and age. I certainly understand the benefits of AI and use it in my professional life. I also used it over the course of this project, but only as a high-level guide and validator of my ideas. I think it's an extremely valuable resource for finding the right learning resources. I don't really think that there's any value in simply vibe-coding the project from beginning to end though, because then nothing was actually learned. Onwards.

## System Design
In my journey into the world of firmware and embedded systems, I have acquired quite a bit of hardware (probably too much). I have had a Raspberry Pi for years and more recently have acquired a few ESP32 boards and an STM32 Nucleo Development Board. I decided that the Pi would be the core "brain" of the operation and the ESP32 would be the "reporter".

## Prediction Algorithm

## Current Functionality Breakdown (V0)

## Roadmap
This is just the beginning.

### V1

### V2

### V3
