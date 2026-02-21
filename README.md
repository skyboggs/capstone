# Capstone Project - Image Tile Analyzer

A C++ graphics application built with raylib that visualizes image tile divisions and provides utilities for tile-based image processing. This project implements a tile generation system useful for sprite sheet manipulation, pixel art analysis, and grid-based image operations.

## Project Overview

This capstone project focuses on creating a robust system for dividing images into tile grids and visualizing those divisions. The application can load images, overlay a pixel-accurate grid, and generate tile data structures that can be used for further image processing operations.

## Core Components

### 1. Tile System (`tile.h` / `tile.cpp`)

The fundamental data structure representing a rectangular tile region.

**Features**:
- Stores tile coordinates (x, y) and dimensions (width, height)
- Multiple constructors for flexible tile creation
- Conversion to raylib Rectangle format via `generateRec()`
- Stream output operator for easy debugging
- Methods for setting dimensions and coordinates

**Example Usage**:
```cpp
tile myTile(10, 20, 32, 32);  // x=10, y=20, width=32, height=32
Rectangle rect = myTile.generateRec();
```

### 2. Tile Utilities (`tileUtils.h` / `tileUtils.cpp`)

Utility functions for tile generation and visual helpers.

**Key Functions**:
- `genTileList()`: Generates a vector of tiles covering an image based on specified tile dimensions
  - Supports custom tile widths and heights
  - Optional mirroring and overlap handling
  - Debug visualization in console
- `checkCLA()`: Command-line argument validation for image file paths
- `drawCheckeredBackground()`: Renders a transparent checkered pattern (like Photoshop's transparency grid)

### 3. Image Viewer (`drawImage.cpp`)

The main application that visualizes images with tile grid overlays.

**Features**:
- Loads and displays images with automatic scaling to fit the screen
- Maintains aspect ratio while maximizing image size (95% of available space)
- Draws pixel-accurate grid overlay showing tile boundaries
- Displays image dimensions in top-left corner
- Shows filename as centered title at top
- Checkered background for transparency visualization
- Command-line support: `./drawImage --image path/to/image.png`

**Visual Elements**:
- Red grid lines showing tile divisions
- Semi-transparent checkered background
- Black/white bordered text boxes for overlays
- 1000x720 window size

### 4. Demo Application (`demoApp.cpp`)

A simple test program to verify raylib installation and basic graphics functionality.

**Features**:
- Renders a centered 10x10 checkered grid
- Interactive: grid scales when spacebar is held
- Mouse tracking with diagonal lines to grid corners
- Useful for testing graphics setup before working with images

### 5. Test Programs

- **tileUtilsTest.cpp**: Unit testing for tile generation functions
- **tileUtilsVisual.cpp**: Visual testing for tile utilities

## Building the Project

The project uses a makefile for compilation. Navigate to the `src/` directory:

```bash
cd src/
make current        # Builds tileUtilsVisual (current development target)
make all           # Builds all executables
make demoApp       # Builds just the demo application
make drawImage     # Builds just the image viewer
make clean         # Removes all built executables
```

**Requirements**:
- C++ compiler (clang++ by default, configurable in makefile)
- raylib library (included in `lib/libraylib.a`)
- macOS frameworks: CoreVideo, IOKit, Cocoa, GLUT, OpenGL

## Running the Applications

### Demo Application
```bash
./demoApp
```

### Image Viewer
```bash
./drawImage                              # Uses default image (assets/City.png)
./drawImage --image assets/Flowers.png   # Load custom image
./drawImage --image path/to/your/image.jpg
```

## Project Structure

```
capstone/
├── header/           # Header files
│   ├── tile.h
│   ├── tileUtils.h
│   └── raylib.h
├── src/              # Source files and makefile
│   ├── tile.cpp
│   ├── tileUtils.cpp
│   ├── drawImage.cpp
│   ├── demoApp.cpp
│   ├── tileTest.cpp
│   ├── tileUtilsTest.cpp
│   ├── tileUtilsVisual.cpp
│   └── makefile
├── lib/              # Libraries
│   ├── libraylib.a
│   └── raylib/       # Full raylib source
├── assets/           # Sample images
│   ├── cat.jpg
│   ├── chat.png
│   ├── City.png
│   ├── Dungeon.png
│   └── Flowers.png
└── tests/            # Test files

```

## Use Cases

- **Sprite Sheet Analysis**: Visualize tile divisions in sprite sheets
- **Pixel Art Tools**: Grid overlay for pixel-perfect editing reference
- **Image Tiling**: Generate tile coordinates for image segmentation
- **Game Development**: Preview tile-based game assets with grid overlays
- **Educational**: Learn about image manipulation and graphics programming

## Technologies Used

- **Language**: C++
- **Graphics Library**: [raylib](https://www.raylib.com/) - A simple and easy-to-use library for game development
- **Build System**: GNU Make
- **Platform**: macOS (portable to other platforms with raylib support)
