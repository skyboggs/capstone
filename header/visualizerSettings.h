#ifndef VISUALIZERSETTINGS_H
#define VISUALIZERSETTINGS_H

#include <iostream>
#include <vector>
#include "raylib.h"
#include "textureMapping.h"
#include "tileUtils.h"
#include "tile.h"

using namespace std;

struct visualizerSettings
{
  visualizerSettings();
  bool drawLines;
  bool drawWithGap;
  bool highlight;
  int gapSize;

  Color highlightColor;
};


#endif
