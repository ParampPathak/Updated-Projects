# Airport Runway Concurrency Simulation

This project implements **Programming Assignment 2: Airport Concurrency**, a multithreaded simulation written in **C** using **POSIX threads (pthreads)**, mutexes, and condition variables.

## Overview
The program simulates an airport with a **single runway** shared by:
- Commercial aircraft
- Cargo aircraft
- Emergency aircraft

An air traffic controller thread coordinates runway access while enforcing safety, fairness, and real-time constraints.

## Key Features
- **Runway capacity enforcement:** Maximum of 2 aircraft on the runway at any time.
- **Mutual exclusion:** Commercial and cargo aircraft are never on the runway simultaneously.
- **Controller fatigue management:** Controller takes a mandatory break after handling 8 aircraft.
- **Fair scheduling:** Prevents starvation by alternating aircraft types after consecutive use.
- **Runway direction control:** Supports NORTH/SOUTH directions with enforced direction switching.
- **Emergency priority:** Emergency aircraft are guaranteed runway access within 30 seconds.
- **Fuel monitoring:** Aircraft dynamically escalate to emergency status if fuel runs low.
- **Deadlock-free design:** Uses condition variables and careful lock management to ensure progress.

## Synchronization
- `pthread_mutex_t` for shared state protection  
- `pthread_cond_t` condition variables for:
  - Commercial aircraft
  - Cargo aircraft
  - Emergency aircraft
- Dedicated controller thread coordinates admission, breaks, and direction switches.

## Files
- `runway.c` — Full simulation implementation
- Input file — Specifies aircraft arrival times, runway usage times, and aircraft types

## Compilation
```bash
make
