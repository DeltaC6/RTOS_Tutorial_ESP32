# FreeRTOS Tutorials - Powered by ESP32

## Introduction

This repository contains series of individual programs, addressing various 
components and concepts of FreeRTOS. The program is written in C language using
Arduino Core Framework.

All the topics covered here were taken from the
**FreeRTOS Tutorial on YouTube** compiled by
**Shawn Hymel - DigiKey Electronics** Please do check out his original series
there. Also please refer to the original work in his
[GIT](https://github.com/ShawnHymel/introduction-to-rtos) repository.

In my opinion this is one of the best exercise to address various RTOS concepts.

## Real Time Operating System - RTOS

The topics covered in this course are in the following order:

| Index | Outline | Description |
|:------|:--------|:------------|
| 1 | Threading | Concurrent tasks creation, scheduling, and operations |
| 2 | Scheduling | Tasks controls, suspend and resume operations |
| 3 | Mem. Management. | Tasks memory allocation, stack and heap organizations |
| 4 | Queue | Data passing operations b/w tasks and memory allocation |
| 5 | Mutex | Secure critical section operation, and common resources |
| 6 | Semaphores | Producer/Consumer model, information relay, and utility for resource control |
| 7 | S/W Timers | One shot and auto reload timers and its various operations |
| 8 | H/W Interrupts | |
| 9 | Advanced Concepts | Discussion on Starvation, Deadlock, Livelock, Priority Inversion (Bounded / UnBounded), and Multi Core Systems |

There is still room for more advanced topics related to operating systems and
various scheduled operations such as atomic operations, multi core systems etc.

## Dependencies

There is no dependencies related to this repository as no external library is
used in compilation of various topics and concepts of FreeRTOS.

## Installations

Arduino is required with ESP32 board installed. Download the Arduino IDE
from the main website. In order to install ESP32 please do the following:

- Step1: Enter following link into the File > Preference > Additional Boards
Manager URL

```
https://dl.espressif.com/dl/package_esp32_index.json
```

- Step2: Go to Tools > Board > Board Manager. Search for ESP32 and install it
- Step3: Select ESP32 Dev Module from Tools > Board > ESP32 Arduino
- Step4: Select COMM port from the Tools > Port
- Step4: Compile and upload

Note:
This project was primarily developed on VS Code IDE with C++ and Arduino
extensions.

## Hardware

The hardware involved for testing and operational deployment is an ESP32
development board. ESP32 is used standalone for the entire implementation of
various topics covered within this repository.

Note: ESP32 comes with a pre configured version of RTOS. If anyone is using
vanilla RTOS there might be several changes related to the functional
implementations within various topics covered. If anyone is trying to use this
repo as an example on some other system such as STM32, or TI boards they can
refer to the [original documentation](Docs/FreeRTOS.pdf) of the freeRTOS.

## Outcome

This repository should be sufficient enough to understand all the basic and
necessary concepts on concurrent programming, and enabling the user to apply
these concepts with their real world projects.

## LICENSES
All code in this repository, unless otherwise noted, is licensed under the
[Zero-Clause BSD / Free Public License 1.0.0 (0BSD)](https://opensource.org/license/0bsd/).

Permission to use, copy, modify, and/or distribute this software for any purpose
with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.

---

Thankyou.
