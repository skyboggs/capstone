#ifndef COMPATABILITYVISTOOLS_H
#define COMPATABILITYVISTOOLS_H

#include <iostream>
#include <vector>
#include "raylib.h"
#include "tileUtils.h"
#include "tile.h"

using namespace std;


struct textureMapping
{
  bool debugMode;
  Vector2 displayDims;
  Vector2 tileDims; // the size / dimensions of the tiles

  Image image;
  Texture2D texture;

  vector<Rectangle> sourceRecs;
  vector<Rectangle> destRecs;

  
  textureMapping(string imagePath,Vector2 tileSize);
  ~textureMapping();
  void setNewTileSize(Vector2);
};

struct visualizerSettings
{
  visualizerSettings();
  bool drawLines;
  bool drawWithGap;
  bool highlight;
  int gapSize;

  Color highlightColor;
};

// checks for user input and updates visualizerSettings if the user activated any shortcut keys
void updateConfig(visualizerSettings& visConfig);

void printVec(const Vector2& v, const string vName);

void drawTileCompatabilities
(
  visualizerSettings visConfig,
  const textureMapping& detailMapping,
  vector<bool> validOptions
);


void generateTileRecs
(
 textureMapping& detailMapper,
 const Vector2& windowDims
);

#endif
