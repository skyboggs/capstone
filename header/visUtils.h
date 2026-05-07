#ifndef COMPATABILITYVISTOOLS_H
#define COMPATABILITYVISTOOLS_H

#include <iostream>
#include <vector>
#include "raylib.h"
#include "textureMapping.h"
#include "visualizerSettings.h"
#include "tileUtils.h"
#include "tile.h"

using namespace std;


void processCommands(string&,int&,int,const char**);

// checks for user input and updates visualizerSettings if the user activated any shortcut keys
void updateConfig(visualizerSettings& visConfig);

void printVec(const Vector2& v, const string vName);

void drawTileCompatabilities
(
  visualizerSettings visConfig,
  const textureMapping& detailMapping,
  vector<bool> validOptions,
  Vector2 offset = {0.0f, 0.0f},
  int selectedIdx = -1
);


void generateTileRecs
(
 textureMapping& detailMapper,
 const Vector2& windowDims
);

// Regenerates only sourceRecs/destRecs for a new window size, without rebuilding the atlas.
void regenTileRecs
(
  textureMapping& detailMapper,
  const Vector2& windowDims
);

#endif
