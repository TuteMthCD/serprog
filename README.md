# Serprog RP2040-W

Serprog is a protocol used to program flash memory over a serial connection. It is commonly used with tools like flashrom to read and write SPI chips. In this project, the RP2040 acts as a programmer that interprets serprog commands sent from the PC.

## Description

This project is based on the RP2040 microcontroller and is developed using the official Pico SDK. It runs FreeRTOS to handle multitasking and ensure efficient execution of the serprog protocol and peripheral control.


### Prerequisites 

Before building and running this project, make sure you have the following tools and dependencies installed:

- **PICO SDK** – Official Raspberry Pi Pico SDK for C/C++ development.  
- **FreeRTOS-Kernel** – Real-time operating system used in the project (SMP branch).  
- **GNU Arm Embedded Toolchain** – To compile the firmware for the RP2040.  
- **CMake** – For configuring and building the project.  
- **Python 3.8+** – Required for tools like `picotool` or flash utilities.  
- **Git** – To clone the repository and manage submodules.
- **flashrom** – Used to communicate with the RP2040 via the serprog protocol.  


### Clone
```bash
git clone https://github.com/TuteMthCD/serprog
```

### Build
```bash
cmake -B build
cd build
make -j$(nproc)
```


### PINS

| Pico Pin | Function |
|----------|----------|
|    17    | CS       |
|    18    | SCK      |
|    19    | MOSI     |
|    16    | MISO     |
