#ifndef TILEUTILS_H
#define TILEUTILS_H

#include <iostream>
#include <vector>
#include "tile.h"
#include "raylib.h"

using std::vector;
using std::string;

// generates a vector<tile> given:
//   int width                - width of image in pixels
//   int height               - height of image in pixels
//   int tileWidth            - tile width in pixels
//   int tileHeight           - tile height in pixels
//   bool mirroredTiles       - mirror the tiles | account for overlap when generating coords
//   bool printGeneratedTiles - print generated tiles in console
vector<tile> genTileList(int,int,int,int,bool,bool,bool);

// inputs:
//   string& argv1     - expects --image or other key commands if added
//   string& argv2     - file path to the custom inputted image
//                       + if the image exists at the path, then it is set at the imagePath
//                       + if the image does NOT exist at the path, then imagePath is set to a default
//   string& imagePath - the set path for the image used
//                       + any failures for checking CLA results in setting the imagePath as default
//
// making sure users use the command line arguments 
// properly for inputting custom images using the following format:
//
// CLA usage:
//   ./drawImage --image path/to/image
//
bool checkCLA(string&,string&,string&);

// inputs:
//   int SCREEN_WIDTH  - dimensions of the users screen in pixels
//   int SCREEN_HEIGHT - dimensions of the users screen in pixels
//   int squareWidth   - dimensions of the tiles being drawn on the screen in pixels
//   int squareHeight  - dimensions of the tiles being drawn on the screen in pixels
void drawCheckeredBackground(int,int, int,Color,Color);

#endif
