# 🖼️ Image Processing System

This project is a **C++ image processing application** that supports two modes of memory management:

- **Conventional mode** using `new` / `delete`
-  **Buddy System** for custom memory allocation

---

## Features Implemented (in Progress)

### 📌 Part 1: Image Loading
- Load an image from the command line (JPG, PNG, BMP, etc.)
- Store the image as a 3D matrix: `pixels[height][width][channels]`
- Display basic image info (dimensions, color channels)

### 📌 Part 4: Memory Management with Buddy System
- Implemented a simplified **Buddy Allocator** for memory allocation
- Toggle between **Buddy System** and **new/delete** with a command-line flag
- Compare performance and allocation behavior
- Print `offsets` when Buddy System is used (for debugging)

## 🔬 Technical Details

### Image Scaling Implementation

The image scaling function uses **bilinear interpolation** to maintain image quality during resizing. Here's how it works:

#### Mathematical Concept
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

#### Implementation Details
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

#### Example
For a scaling factor of 2.0:
```
Original Image (4x4)     Scaled Image (8x8)
+--------+              +----------------+
|  P00   |              |   P00'  P01'   |
|  P10   |    →         |   P10'  P11'   |
+--------+              +----------------+
```

Where each new pixel (P') is calculated using the bilinear interpolation formula.

## 🔧 Project Structure
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
│       └── test.jpg
│
├── build/                # Compiled executable
│
├── output/              # Processed images output
│   └── *.png           # Generated output images
│
└── Makefile
```

## 📝 Usage

### Building the Project
```bash
make all          # Build the project and create necessary directories
make clean        # Clean all built files and outputs
```

### Basic Operations
```bash
# Invert image colors
make invertir

# Scale image to double size
make escalar_2x

# Scale image to half size
make escalar_mitad

# Run all example operations
make test_all
```

### Custom Operations
```bash
# Custom inversion
make run ARGS="input.jpg output/result.png invertir -buddy"

# Custom scaling (e.g., scale by 1.5)
make run ARGS="input.jpg output/result.png escalar 1.5 -buddy"
```

### Command Line Format
```bash
./build/image-processing-system <input_image> <output_image> <operation> [scale_factor] <memory_mode>

# Operations:
- invertir              # Invert colors
- escalar <factor>      # Scale image by factor

# Memory Modes:
- -buddy               # Use Buddy System allocator
- -no-buddy            # Use conventional allocation
```

## 🔍 Output
- All processed images are saved in the `output/` directory
- The program displays:
  - Image dimensions and channels
  - Processing time
  - Output file location
  - Memory allocation mode used

## 💡 Examples
1. **Invert Colors**:
   ```bash
   make invertir
   # Output: output/salida_invertida.png
   ```

2. **Double Size**:
   ```bash
   make escalar_2x
   # Output: output/salida_2x.png
   ```

3. **Half Size**:
   ```bash
   make escalar_mitad
   # Output: output/salida_mitad.png
   ```

4. **Custom Scale**:
   ```bash
   make run ARGS="test/testImg/test.jpg output/custom_scale.png escalar 1.5 -buddy"
   ```
