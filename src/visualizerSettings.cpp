#include <iostream>
#include <vector>
#include "visUtils.h"
#include "raylib.h"
#include "tileUtils.h"
#include "visualizerSettings.h"
#include "tile.h"

using namespace std;

visualizerSettings::visualizerSettings()
  :
    drawLines(true),
    highlight(true),
    highlightColor(WHITE)
{
};


