# Adversarial Search Techniques for Approximating the One-Dimensional Bin Packing Problem

This repository contains the code for the MSc project titled: "Adversarial Search Techniques for Approximating the One-Dimensional Bin Packing Problem."

## Installation

### Prerequisites

Make sure you have the following installed on your system:

- CMake
- Python 3
- G++

### Easy Testing

Follow these steps for easy testing of the project:

1. **Download the Repository**: Clone or download the repository and navigate to the parent directory.

2. **Build the Project**: Run the following commands:

    ```bash
    cd build
    cmake ..
    cmake --build .
    ```

3. **Run the Application**: Execute the following command to run the application:

    ```bash
    ./BinPackingGame <Bin Capacity> <Best Known Solution> <List of Items (Comma-Separated)> <Name of Algorithm> <Name of Data Structure> <Y/N>
    ```

4. **Test with Jupyter Notebook**: You can also test the application using the provided Jupyter notebook:

    ```bash
    jupyter notebook ../tests/test.ipynb
    ```

### Full Testing

For a complete testing experience, follow these steps:

1. **Build the Project**: Repeat steps 1 and 2 from the Easy Testing section.

2. **Make the Application Executable**: Run the following command:

    ```bash
    chmod +x ./build/BinPackingGame
    ```

3. **Run the Application with Python**: Start the application by running:

    ```bash
    python3 app.py
    ```

4. **Configure Visualization**: Define the content of `visualization.py` for plotting data. Examples are provided in `example.ipynb`.

5. **Configure Statistical Analysis**: Define the content of `statistical_analysis.py` for statistical analysis.
