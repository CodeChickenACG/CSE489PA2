# CSE 489/589: Programming Assignment 2 - Reliable Transport Protocols

## Objectives
In this assignment, we aim to implement three reliable data transport protocols in a given simulator:
- Alternating-Bit (ABT)
- Go-Back-N (GBN)
- Selective-Repeat (SR)

## Getting Started

### Reading
- Alternating-Bit Protocol (rdt3.0): 8th Edition Textbook, Page 241 – Page 245
- Go-Back-N Protocol: 8th Edition Textbook, Page 245 – Page 250
- Selective-Repeat Protocol: 8th Edition Textbook, Page 250 – Page 256

### Play with the GBN and SR applet
- [GBN and SR Applet](https://www2.tkn.tu-berlin.de/teaching/rn/animations/gbn_sr/)

### Programming environment
- Write in C code, compatible with the GCC environment.
- The program should be able to make and build in any Linux environment.
- Avoided disk I/O.

### Overview
- Implement sending and receiving code for the protocols.
- The code will execute in a simulated hardware/software environment.

### Routines to write
- A_output(message)
- A_input(packet)
- A_timerinterrupt()
- A_init()
- B_input(packet)
- B_init()

### Software Interfaces
- starttimer(calling_entity, increment)
- stoptimer(calling_entity)
- tolayer3(calling_entity, packet)
- tolayer5(calling_entity, data)
- getwinsize()
- get_sim_time()

## Run and build the program
-Change Directory to /cse489589_assignment2/zhibiaoj
-In terminal run make to build the executable files and make clean to clear all files created by make
-Change directory to /cse489589_assignment2/grader
-There are 3 tests for each protocols
### Testing
- SANITY Tests- In terminal run ./sanity_tests -h
  Sanity tests are designed to ensure the basic correctness and stability of your protocol implementations.
- BASIC Tests - In terminal run ./basic_tests -h
  Basic tests are designed to test protocols' abilities to handle more realistic network conditions. These may include:
- ADVANCED Tests - In terminal run ./advanced_tests -h
  Advanced tests are more complex and are designed to challenge the protocols under harsher, more diverse network conditions.
  
