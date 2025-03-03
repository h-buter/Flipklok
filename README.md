# Flipklok in 1970s Style with DCF77 Time Synchronization

Welcome to the repository for my project as part of my Bachelor's degree in Electronics at Windesheim. This
project involves creating a flip clock inspired by the design from the 1970s, which is synchronized using time
information from the DCF77 long-wave time signal. The goal is to ensure that this electronic and electromechanical
device operates efficiently within a low power environment.

At time of writing I am still deciding on if the clock will be used a desk clock or if I build it in to my 1977 Relaint Robin MK1
This is why some design decisions, like low power, are not always directly clear in the scope of a normal clock project.

## Project Overview

This flip clock project integrates both mechanical components and electronics to display the current time
accurately based on data received from the DCF77 transmitter located in Germany. Designed with energy efficiency
in mind, the clock maintains continuous time tracking to compensate for any potential reception limitations of the
DCF77 signal while minimizing power consumption.

## Key Challenges

- **Mechanical and Time Management**: The project requires careful application of mechanics and efficient time
management techniques to ensure the device functions optimally on a 12V source.

- **Integration of Electronics and Electromechanics**: Incorporating electronic components like motors and sensors
for position detection into the mechanical design poses significant challenges.

- **PCB Design**: The creation of a custom PCB is necessary to accommodate all electronic components while
maintaining compactness and efficiency.

- **DCF77 Protocol Implementation**: Applying the DCF77 protocol ensures accurate time synchronization. This
involves handling signals and data correctly within the system’s constraints.

- **Software Development**: The software component, developed in C with some sections written in assembly
language, is responsible for processing DCF77 data and controlling stepper or servo motors to operate the flip
clock.

## Project Boundaries

### Software
The software aspect focuses on:
- Processing DCF77 signal data.
- Controlling a stepper/servo motor mechanism to display time changes accurately.

### Hardware
The hardware component involves:
- Integrating electronic and electromechanical components for seamless operation of the flip clock module.
- Receiving and processing DCF77 timing information effectively.

## Getting Started

To get started with this project, you need to:


## Contribution

Contributions are welcome! Whether it's optimizing the code for better performance or suggesting improvements in
hardware integration, your input is valuable.

## License

This project is licensed under to be determined 

---

Thank you for exploring this unique blend of vintage design and electronics. Should you have any questions
or suggestions regarding this project, feel free to open an issue or contact me directly.