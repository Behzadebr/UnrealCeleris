# Unreal Engine 5.3 Wave Simulator Project - Celeris2024

## Overview
The Unreal Engine 5.3 Wave Simulator Project, part of the Celeris2023 initiative, aims to model wave dynamics with high accuracy using advanced computational technologies. This project focuses on writing shader code and GPU programming for real-time and faster-than-real-time simulations. Key objectives include capturing detailed wave dynamics, achieving efficient computational performance, and providing a user-friendly interface for simulation control and visualization.


## Simulation Architecture
### Initial Setup and Data Generation
- **Depth Surface and Ocean Elevation Initialization:** Generate a depth surface representing the ocean's bathymetry and an initial ocean elevation state.
- **Wave Parameter Initialization:** Initialize wave characteristics based on predefined or user-specified conditions.

### Compute Shader Pipeline
- **Pass1:** State reconstruction and preliminary calculations for water dynamics.
- **Pass2:** Flux calculation and adjustment in the x and y directions.
- **Pass3:** State update using NLSW and Boussinesq methods, handling wave dispersion and non-linearity.
- **Boundary Handling:** Special handling for simulation boundaries, including walls, sponge layers, and wave generation.
- **Tridiagonal Matrix Solver (PCR):** Efficiently handle tridiagonal matrix systems for Boussinesq simulations.

### Rendering and Visualization
- Render the simulated ocean surface, converting data into visual forms for real-time visualization and analysis.

## Implementation Details
### Texture Management
- **Texture Initialization:** Initialize textures for bathymetry, initial ocean state, and dynamic simulation data.
- **Data Encoding in Textures:** Utilize compute shaders to encode simulation data into textures for efficient GPU processing.

### Simulation Parameters and Configuration
- **Environmental Settings:** Define computational domain size, cell size, and boundary conditions.
- **Physical Properties:** Control base depth, friction parameters, and dispersion characteristics.
- **Time Integration Scheme:** Choose from Euler Integration, Third-order Adams-Bashforth Predictor, or Fourth-order Predictor-Corrector.

## Contributions and Innovations
- **Efficient GPU Utilization:** Leverage modern GPUs for high-speed and accurate wave simulations.
- **High Level of User Control and Interactivity:** Offer control and interactivity for research and training purposes.
- **Modular Architecture:** Facilitate integration with various water rendering systems.
- **Real-Time and Predictive Simulation Capabilities:** Enable real-time and faster-than-real-time simulations for predictive modeling.

## Getting Started
1. **Clone the Repository:**
    ```sh
    git clone https://github.com/Behzadebr/UnrealCeleris.git
    ```
2. **Open the Project:**
    - Open Unreal Engine 5.3.
    - Load the project from the cloned directory.
3. **Run the Simulation:**
    - Configure simulation parameters in the provided JSON files.
    - Execute the simulation within the Unreal Engine editor.

## Contributions
- **Behzad Ebrahimi:** Project lead, focusing on simulation architecture, compute shaders, and GPU utilization.

## Contact
For questions, please contact Behzad Ebrahimi at bebrahim@usc.edu

## License
This project is licensed under the MIT License - see the LICENSE file for details.
