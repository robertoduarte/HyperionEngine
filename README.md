# HyperionEngine

The **HyperionEngine** project is designed to provide a comprehensive set of tools for developing ECS (Entity-Component-System) Architecture based games specifically for the Sega Saturn. 

## Dependencies

The project heavily relies on the **libyaul** library, which can be found at [https://github.com/yaul-org/libyaul](https://github.com/yaul-org/libyaul). This library is considered the most complete toolset available for Sega Saturn game development.

## Goals

The main goal of the HyperionEngine project is to leverage the advanced features introduced in C++20 and the ECS architecture to optimize game development for the Sega Saturn. The specific goals are as follows:

1. **Efficient Performance**: By utilizing compile-time computations and minimizing runtime operations, the project aims to maximize performance on the Sega Saturn. This is crucial because the Sega Saturn is an embedded system with significantly lower specifications compared to modern standards.

2. **Modularity and Scalability**: The ECS architecture promotes a modular and scalable approach to game development. HyperionEngine provides a robust framework for managing entities, components, and systems, allowing developers to easily add, remove, and update game entities without sacrificing performance.

3. **Simplified Development Process**: The ECS pattern separates game logic from entity data, making the development process more streamlined and maintainable. HyperionEngine provides a set of tools and abstractions to facilitate the implementation of game features, reducing complexity and improving code organization.

4. **Platform Optimization**: By specifically targeting the Sega Saturn, HyperionEngine takes advantage of the system's unique characteristics and limitations. It provides custom implementations of standard library and standard template library features, optimized for the Sega Saturn environment, to ensure efficient resource utilization.

## Approach

To achieve the desired optimization and meet the goals mentioned above, the HyperionEngine project avoids using the standard library and standard template library (except for the type traits header). Instead, equivalent tools and functionalities are implemented from scratch, carefully considering the limitations of the Sega Saturn system.

By following this approach, the HyperionEngine project provides a tailored solution for Sega Saturn game development, optimizing performance while working within the constraints of the platform. Developers can leverage the power of ECS architecture and the project's features to create efficient and scalable games for the Sega Saturn.
