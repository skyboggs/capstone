#ifndef TILEUTILS_H
#define TILEUTILS_H

#include <iostream>
#include <vector>
#include "tile.h"
#include "raylib.h"

using std::vector;
using std::string;

// generates a vector<tile> given:
// int width  of image
// int height of image
// int tileSize in a square shape
// bool mirror the tiles | account for overlap when generating coords
// bool print generated tiles in console
// testing the program ( removes colored output )
vector<tile> genTileList(int,int,int,int,bool,bool,bool);
bool checkCLA(string&,string&,string&);
void drawCheckeredBackground(int,int, int,Color,Color);

#endif
