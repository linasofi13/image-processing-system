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
├── include/
│   ├── imagen.h
│   └── buddy_allocator.h
│
├── src/
│   ├── main.cpp
│   ├── imagen.cpp
│   ├── buddy_allocator.cpp
│   └── stb_wrapper.cpp
│
├── test/
│   └── testImg/
│       └── test.jpg
│
├── build/
│   └── (compiled executable )
│
├── resultados/
│   └── 
│
└── Makefile
```

