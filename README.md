# Logistic Simulation Project

## Overview

The Logistic Simulation project is designed to simulate logistics operations, including order processing and scheduling. It utilizes a scheduler and a shell interface to manage tasks and interact with users in real-time.

## Build Instructions

### Prerequisites

- CMake 3.10 or higher
- A C++17 compatible compiler

### Building the Project

To build the project, you can use the provided shell script for Unix-like systems or the batch script for Windows systems.

#### Unix-like Systems

```bash
./build.sh
```

#### Windows Systems

```cmd
build.bat
```

Alternatively, you can use CMake directly:

```bash
mkdir build
cd build
cmake ..
make
```

## Running the Project

After building the project, you can run the executable from the `bin` directory:

```bash
./bin/LogisticSimulation
```

## Real-time Shell Usage

The project includes a shell interface that allows for real-time interaction. Once the simulation is running, you can use the shell to monitor and control the simulation process.

## Configuration

### CLI Parameters

The project accepts command-line arguments to configure its behavior. These arguments are processed by the `Initializer` class. Ensure to pass the necessary parameters when executing the program.

### JSON Configuration

The project uses several JSON configuration files to load data:

- `config/configuration.json`: Main configuration file that references other JSON files.
- `config/references.json`: Contains reference data.
- `config/products.json`: Contains product data.
- `config/orders.json`: Contains order data.
- `config/parameters.json`: Contains simulation parameters.
- `config/logistics.json`: Contains logistics data.

Ensure these files are correctly configured before running the simulation.

## License

This project is licensed under the MIT License.
