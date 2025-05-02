## 🖼️ Image Processing System

This project is a **C++ image processing application** that supports two modes of memory management:

- **Conventional mode** using `new` / `delete`
-  **Buddy System** for custom memory allocation

---
### Table of Contents
- [Features Implemented](#-features-implemented)
  - [📌 Part 1: Image Loading](#-part-1-image-loading)
  - [📌 Part 2: Rotate an Image in the Center](#-part-2-rotate-an-image-in-the-center)
  - [📌 Part 3: Scaling an Image](#-part-3-scaling-an-image)
  - [📌 Part 4: Memory Management with Buddy System](#-part-4-memory-management-with-buddy-system)
  - [📌 Part 5: Performance and Memory Consumption Comparison](#-part-5-performance-and-memory-consumption-comparison)
  - [📌 Part 6: Data Output](#-part-6-data-output)
- [🔬 Technical Details](#-technical-details)
  - [Image Scaling Implementation](#image-scaling-implementation)
  - [Performance Measurements](#performance-measurements)
  - [Concurrency with OpenMP](#concurrency-with-openmp)
- [Project Structure](#project-structure)
- [📝 Usage](#-usage)
  - [Building the Project](#building-the-project)
  - [Basic Operations](#basic-operations)
  - [Custom Operations](#custom-operations)
  - [Command Line Format](#command-line-format)
- [🔍 Output](#-output)
- [💡 Examples](#-examples)
- [Contributors](#contributors)
---

### Features Implemented

#### 📌 Part 1: Image Loading
- Load an image from the command line (JPG, PNG, BMP, etc.)
- Store the image as a 3D matrix: `pixels[height][width][channels]`
- Display basic image info (dimensions, color channels)

#### 📌 Part 2: Rotate an Image in the Center
- Allow the user to enter the rotation angle.
- Implement a rotation algorithm using bilinear interpolation to preserve image quality.
- The rotation must be performed on the center of the image.
- If the rotated image generates empty spaces, they should be filled by a constant value (e.g., black or white).

#### 📌 Part 3: Scaling an Image
- Allow the user to enter the scaling factor (greater or less than 1.0).
- Use bilinear interpolation to resize the image.
- Maintain image aspect ratio during scaling.
- Display information about the new image size after the operation.

#### 📌 Part 4: Memory Management with Buddy System
- Implemented a simplified **Buddy Allocator** for memory allocation
- Toggle between **Buddy System** and **new/delete** with a command-line flag
- Compare performance and allocation behavior
- Print `offsets` when Buddy System is used (for debugging)

#### 📌 Part 5: Performance and Memory Consumption Comparison
- Measure processing time for rotation and scaling operations using `std::chrono`.
- Measure memory consumption using `mallinfo()` or `getrusage()` to determine the size of reserved memory.
- Display on screen a direct comparison between performance and memory consumption in the two allocation modes.

#### 📌 Part 6: Data Output
The program must present a clear output with the following data:
- Original and final dimensions of the image.
- Rotation angle applied.
- Processing time in milliseconds.
- Memory consumption in both modes (Buddy System and conventional).
- Performance difference between both modes.

### 🔬 Technical Details

#### Image Scaling Implementation

The image scaling function uses **bilinear interpolation** to maintain image quality during resizing. Here's how it works:

##### Mathematical Concept
For a scaling factor $s$, the new dimensions are calculated as:
```math
\text{new\_width} = \lfloor \text{width} \times s \rfloor \\
\text{new\_height} = \lfloor \text{height} \times s \rfloor
```

For each pixel in the new image at position $(x, y)$, we find its corresponding position in the original image:
```math
\text{orig\_x} = \frac{x}{s} \\
\text{orig\_y} = \frac{y}{s}
```

The bilinear interpolation uses four neighboring pixels to compute the new pixel value:
```math
P(x,y) = (1-dx)(1-dy)P_{00} + dx(1-dy)P_{10} + (1-dx)dyP_{01} + dx\cdot dy\cdot P_{11}
```

Where:
- $P_{00}, P_{10}, P_{01}, P_{11}$ are the four neighboring pixels
- $dx, dy$ are the fractional parts of the original coordinates

##### Implementation Details
1. **Memory Allocation**:
   - Creates a new 3D matrix for the scaled image
   - Supports both Buddy System and conventional memory allocation
   - Properly handles memory cleanup

2. **Interpolation Process**:
   - Maps each new pixel to its position in the original image
   - Calculates weights for the four neighboring pixels
   - Applies interpolation to each color channel independently

3. **Edge Handling**:
   - Uses `std::min` to prevent accessing pixels outside image boundaries
   - Maintains color accuracy at image edges

##### Example
For a scaling factor of 2.0:
```
Original Image (4x4)     Scaled Image (8x8)
+--------+              +----------------+
|  P00   |              |   P00'  P01'   |
|  P10   |    →         |   P10'  P11'   |
+--------+              +----------------+
```

Where each new pixel (P') is calculated using the bilinear interpolation formula.

#### Performance Measurements

The system includes comprehensive performance monitoring for both memory allocation modes:

##### Metrics Measured
1. **Processing Time**:
   - Measured using `std::chrono::high_resolution_clock`
   - Reported in milliseconds
   - Includes both computation and memory allocation time

2. **Memory Usage**:
   - Measured using `mallinfo2()` for conventional allocation
   - Tracks memory allocation and deallocation
   - Reported in kilobytes

3. **CPU Usage**:
   - User time: CPU time spent in user code
   - System time: CPU time spent in system calls
   - Measured using `getrusage()`
   - Reported in milliseconds

##### Example Output
```
[INFO] Escalado de imagen (factor 2.0):
  Tiempo de procesamiento: 150 ms
  Memoria utilizada: 2048 KB
  CPU User: 145 ms
  CPU System: 5 ms
  Nuevas dimensiones: 800x600
```

##### Performance Comparison
The system allows direct comparison between Buddy System and conventional allocation:

1. **Memory Allocation**:
   - Buddy System: More efficient for large allocations
   - Conventional: Better for small, frequent allocations

2. **Processing Time**:
   - Buddy System: Faster for large images due to optimized allocation
   - Conventional: More predictable performance across different image sizes

3. **CPU Usage**:
   - Buddy System: Lower system time due to custom allocation
   - Conventional: Higher system time due to OS memory management

#### Concurrency with OpenMP

To improve performance, particularly for computationally intensive tasks like image scaling, the project utilizes **OpenMP** for parallel processing.

- **Targeted Parallelization**: The primary loop within the `escalarImagen` function (responsible for bilinear interpolation) is parallelized using the `#pragma omp parallel for` directive.
- **Mechanism**: This directive instructs the compiler to distribute the iterations of the outer loop (iterating over image rows) across multiple available processor cores. Each thread processes a subset of the rows independently.
- **Benefit**: This significantly reduces the execution time for scaling large images by leveraging multi-core CPU architectures. The speedup is most noticeable on systems with multiple cores.

##### Benchmark Example

A simple benchmark scaling the `test/testImg/test.png` image (540x540) by a factor of 2.0 using conventional memory allocation (`-no-buddy`) shows the following processing times for the scaling operation itself:

- **Multi-threaded (Default OpenMP)**: ~69 ms
- **Single-threaded (`OMP_NUM_THREADS=1`)**: ~84 ms

*Note: Actual times may vary based on the system's CPU and current load.*

### Project Structure
```
image-processing-system/
│
├── include/               # Header files
│   ├── imagen.h          # Image processing class definition
│   └── buddy_allocator.h # Memory allocator implementation
│
├── src/                  # Source files
│   ├── main.cpp
│   ├── imagen.cpp
│   ├── buddy_allocator.cpp
│   └── stb_wrapper.cpp
│
├── test/                 # Test images
│   └── testImg/
│       ├── image.png
│       ├── image2.png
│       ├── test.png
│       └── test2.jpg
│
├── build/                # Compiled executable
│
├── output/              # Processed images output
│   └── *.png           # Generated output images
│
└── Makefile
```
### 🎞️ Screenshots

![image](https://github.com/user-attachments/assets/d866809f-3584-4145-82c5-48d597afd1e8)
![image](https://github.com/user-attachments/assets/e7bbb1e1-a2d7-4db2-b3f2-af4be1414ac2)
![image](https://github.com/user-attachments/assets/5f73199d-bbac-4b2a-ae03-33b8bda30aae)

### 📝 Usage

#### Building the Project
```bash
make all          # Build the project and create necessary directories
make clean        # Clean all built files and outputs
```

#### Basic Operations
```bash
# Scale image to double size
make escalar_2x

# Scale image to half size
make escalar_mitad

# Rotate image by 45 degrees
make rotar

# Run all example operations
make test_all
```

#### Custom Operations
```bash
# Custom scaling (e.g., scale by 1.5)
make run ARGS="input.jpg output/result.png escalar 1.5 -buddy"

# Custom rotation (e.g., rotate by 30 degrees)
make run ARGS="input.jpg output/result.png rotar 30 -buddy"
```

#### Command Line Format
```bash
./build/image-processing-system <input_image> <output_image> <operation> [parameters] <memory_mode>

# Operations:
- escalar <factor>      # Scale image by factor
- rotar <angle>         # Rotate image by angle in degrees

# Memory Modes:
- -buddy               # Use Buddy System allocator (will also simulate and compare with conventional)
- -no-buddy            # Use conventional allocation only
```

### 🔍 Output
- All processed images are saved in the `output/` directory
- The program displays:
  - Image dimensions and channels
  - Operation parameters (scale factor or rotation angle)
  - Processing time comparison between memory allocation methods
  - Output file location

### 💡 Examples
1. **Scale Image**:
   ```bash
   make escalar_2x
   # Output: output/salida_2x.png
   ```

2. **Rotate Image**:
   ```bash
   make rotar
   # Output: output/salida_rotada.png
   ```

3. **Compare Memory Systems**:
   ```bash
   make run ARGS="test/testImg/test.png output/comparison.png escalar 1.5 -buddy"
   # This will process the image with both Buddy System and conventional allocation
   # and display a performance comparison
   ```
### Contributors

<a href="https://github.com/linasofi13/image-processing-system/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=linasofi13/image-processing-system" />
</a>
