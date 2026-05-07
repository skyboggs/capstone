#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "raylib.h"
#include "tile.h"
#include "cell.h"
#include "textureMapping.h"

using namespace std;


cell::cell()
	:
		isSelected(false),
		generateRoughColor(true),
		pixelTolerence(0.0f),
		selectedColor(RED),
		selectedTile(tile(0,0,1,1)),
		possibleTiles(vector<bool>()),
		validIndexes(vector<int>()),
		roughColor(BLACK)
{
}

cell::cell(bool genRoughColor)
	:
		isSelected(false),
		generateRoughColor(genRoughColor),
		pixelTolerence(0.0f),
		selectedColor(RED),
		selectedTile(tile()),
		possibleTiles(vector<bool>()),
		validIndexes(vector<int>()),
		roughColor(BLACK)
{
}

void cell::init(const textureMapping& texMap)
{
  int tileCount = (int)texMap.sourceRecs.size();
  possibleTiles.assign(tileCount, true);
  validIndexes.resize(tileCount);
  for(int i = 0; i < tileCount; ++i) { validIndexes[i] = i; }
}


Color cell::getCellColor()
{
	// returning the current selected color if it 
	// has been selected
	if(isSelected) { return selectedColor; }

	return roughColor;
}

void cell::pickTile(const textureMapping& texMap)
{
  if(validIndexes.empty()) { return; }

  int pickedIdx    = validIndexes[rand() % validIndexes.size()];
  int centerOffset = (texMap.tileDims - 1) / 2;

  selectedColor = texMap.tilePixels[pickedIdx][centerOffset * texMap.tileDims + centerOffset];
  isSelected = true;

  // collapse: only the picked tile remains valid
  fill(possibleTiles.begin(), possibleTiles.end(), false);
  possibleTiles[pickedIdx] = true;
  validIndexes = { pickedIdx };
}

void cell::pickSpecificTile(int tileIdx, const textureMapping& texMap)
{
  int centerOffset = (texMap.tileDims - 1) / 2;
  selectedColor = texMap.tilePixels[tileIdx][centerOffset * texMap.tileDims + centerOffset];
  isSelected = true;

  fill(possibleTiles.begin(), possibleTiles.end(), false);
  possibleTiles[tileIdx] = true;
  validIndexes = { tileIdx };
}

// this updates what tiles are now allowed at the current position
bool cell::updateTiles(Vector2 offset, Color newColor, const textureMapping& texMap)
{
  // iterate validIndexes backwards so erasing by position i stays correct
  for(int i = (int)validIndexes.size() - 1; i >= 0; --i)
  {
    int idx = validIndexes[i];
    const Color& tileColor = texMap.tilePixels[idx][(int)offset.y * texMap.tileDims + (int)offset.x];

    float dr = (float)std::abs((int)newColor.r - (int)tileColor.r) / 255.0f;
    float dg = (float)std::abs((int)newColor.g - (int)tileColor.g) / 255.0f;
    float db = (float)std::abs((int)newColor.b - (int)tileColor.b) / 255.0f;
    float dist = (dr + dg + db) / 3.0f;

    if(dist > pixelTolerence)
    {
      possibleTiles[idx] = false;
      validIndexes[i] = validIndexes.back();
      validIndexes.pop_back();
    }
  }

  return !validIndexes.empty();
}

bool cell::updateTilesCompatibleWith(const cell& source, int dx, int dy, const textureMapping& texMap)
{
  const int tileDims = texMap.tileDims;

  // overlap region expressed in the SOURCE tile's coordinate space
  int ox_lo = max(0, dx);
  int ox_hi = min(tileDims - 1, tileDims - 1 + dx);
  int oy_lo = max(0, dy);
  int oy_hi = min(tileDims - 1, tileDims - 1 + dy);

  bool anyEliminated = false;

  for(int i = (int)validIndexes.size() - 1; i >= 0; --i)
  {
    int myIdx = validIndexes[i];

    // keep this tile if ANY source tile is pixel-perfect compatible with it
    bool hasCompatible = false;
    for(int j = 0; j < (int)source.validIndexes.size() && !hasCompatible; ++j)
    {
      int srcIdx = source.validIndexes[j];

      bool match = true;
      for(int oy = oy_lo; oy <= oy_hi && match; ++oy)
      {
        for(int ox = ox_lo; ox <= ox_hi && match; ++ox)
        {
          const Color& srcColor = texMap.tilePixels[srcIdx][oy * tileDims + ox];
          const Color& myColor  = texMap.tilePixels[myIdx][(oy - dy) * tileDims + (ox - dx)];
          if(srcColor.r != myColor.r ||
             srcColor.g != myColor.g ||
             srcColor.b != myColor.b) { match = false; }
        }
      }
      if(match) { hasCompatible = true; }
    }

    if(!hasCompatible)
    {
      possibleTiles[myIdx] = false;
      validIndexes[i] = validIndexes.back();
      validIndexes.pop_back();
      anyEliminated = true;
    }
  }

  return anyEliminated;
}

void cell::updateRoughColor(const textureMapping& texMap)
{
  if(!generateRoughColor || validIndexes.empty()) { return; }

  Vector3 colorTotals{0.0f, 0.0f, 0.0f};

  int centerOffset = (texMap.tileDims - 1) / 2;

  for(int i = 0; i < (int)validIndexes.size(); ++i)
  {
    int idx = validIndexes[i];
    const Color& c = texMap.tilePixels[idx][centerOffset * texMap.tileDims + centerOffset];
    colorTotals.x += c.r;
    colorTotals.y += c.g;
    colorTotals.z += c.b;
  }

  float count = (float)validIndexes.size();
  roughColor = {
    (unsigned char)(colorTotals.x / count),
    (unsigned char)(colorTotals.y / count),
    (unsigned char)(colorTotals.z / count),
    255
  };
}

ostream& operator<<(ostream& os, const cell& rhs)
{
	os << "-- Cell --" << endl;
	os << "isSelected         : " << rhs.isSelected  << endl;
	os << "generateRoughColor : " << rhs.generateRoughColor << endl;
	os << "pixelTolerence     : " << rhs.pixelTolerence << endl;
	os << "selectedColor      : " ;
	os << "( ";
	os << to_string(rhs.selectedColor.r) << ", ";
	os << to_string(rhs.selectedColor.g) << ", ";
	os << to_string(rhs.selectedColor.b);
	os << " )" << endl;
	os << "selectedTile       : " << (rhs.selectedTile + "") << endl;
	os << "----------" << endl;
	return os;
}
