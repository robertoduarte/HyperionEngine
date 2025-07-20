# HyperionEngine (Archived)

> **Note**: This project has been archived as its development has been moved to more focused repositories. The ECS core functionality has been moved to the [SECS](https://github.com/robertoduarte/SECS) project.

## Project Status: Archived

Development on HyperionEngine has been discontinued as its features have been split into more specialized projects. The ECS core functionality has been moved to [SECS](https://github.com/robertoduarte/SECS) (Small Entity Component System).

## Successor Projects

HyperionEngine served as the foundational base for several other Saturn development libraries:

- [SECS](https://github.com/robertoduarte/SECS) - Small Entity Component System
- [SaturnRingLib](https://github.com/ReyeMe/SaturnRingLib) - Saturn development utilities
- [SaturnMathPP](https://github.com/robertoduarte/SaturnMathPP) - Mathematical operations for Saturn

---

## Original Project Description

The **HyperionEngine** project was designed to provide a comprehensive set of tools for developing ECS (Entity-Component-System) Architecture based games specifically for the Sega Saturn. 

### Dependencies

The project heavily relied on the **libyaul** library, which can be found at [https://github.com/yaul-org/libyaul](https://github.com/yaul-org/libyaul). This library is considered the most complete toolset available for Sega Saturn game development.

### Goals

The main goal of the HyperionEngine project was to leverage the advanced features introduced in C++20 and the ECS architecture to optimize game development for the Sega Saturn. The specific goals were as follows:

1. **Efficient Performance**: By utilizing compile-time computations and minimizing runtime operations, the project aimed to maximize performance on the Sega Saturn.

2. **Modularity and Scalability**: The ECS architecture promoted a modular and scalable approach to game development.

3. **Simplified Development Process**: The ECS pattern separated game logic from entity data, making the development process more streamlined and maintainable.

4. **Platform Optimization**: The project provided custom implementations optimized for the Sega Saturn environment.
