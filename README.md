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

---

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
