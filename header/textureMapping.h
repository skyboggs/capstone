#ifndef TEXTUREMAPPING_H
#define TEXTUREMAPPING_H

#include <iostream>
#include <vector>
#include "raylib.h"
#include "tile.h"

using namespace std;


struct textureMapping
{
  bool debugMode;

  bool genRotatedTiles; // adds a x 4 multiplier to the total number of tiles in the image
  bool genMirroredX;    // adds a x 2 multiplier to the amount of tiles
  bool genMirroredY;    // adds a x 2 multiplier to the amount of tiles
  bool genOverlappingX; // adds a (tileWidth  - 1) x M where M is our tiles multiplied amount of tiles
  bool genOverlappingY; // adds a (tileHeight - 1) x M where M is our tiles multiplied amount of tiles

  bool usingOverlappingImage;

  int tileDims; // the size / dimensions of the tiles

  Vector2 displayDims;
  Vector2 imageDims;
  Vector2 tileCount;

  Image image;
  Texture2D texture;
  Image tileData;
  Texture2D tileAtlas;

  vector<Rectangle> sourceRecs;
  vector<Rectangle> destRecs;

  
  textureMapping(string imagePath,int tileSize);
  ~textureMapping();
  void setNewTileSize(int);
};


#endif
