#include <iostream>
#include <algorithm>
#include <queue>
#include <cstdlib>
#include "raylib.h"
#include "tablet.h"
#include "textureMapping.h"

using namespace std;

tablet::tablet(Vector2 tabletDimensions, Vector2 screenDimension)
  :
    tabletDims(tabletDimensions),
    pixelDims({screenDimension.x / tabletDimensions.x, screenDimension.y / tabletDimensions.y}),
    tabletPixels((int)tabletDimensions.y, vector<cell>((int)tabletDimensions.x)),
    tabletScreen(LoadRenderTexture((int)screenDimension.x, (int)screenDimension.y)),
    cellQueueCount((int)tabletDimensions.y, vector<int>((int)tabletDimensions.x, -1))
{
}

void tablet::updateTexture()
{
  BeginTextureMode(tabletScreen);
  ClearBackground(BLACK);

  for(int row = 0; row < (int)tabletDims.y; ++row)
  {
    for(int col = 0; col < (int)tabletDims.x; ++col)
    {
      DrawRectangleRec(
        Rectangle{col * pixelDims.x, row * pixelDims.y, pixelDims.x, pixelDims.y},
        tabletPixels[row][col].getCellColor()
      );
    }
  }

  EndTextureMode();
}

void tablet::updateCell(Vector2 loc, Vector2 pixelOffset, Color newColor, const textureMapping& texMap)
{
  int row = (int)loc.y;
  int col = (int)loc.x;
  cell& c = tabletPixels[row][col];

  c.updateTiles(pixelOffset, newColor, texMap);
  c.updateRoughColor(texMap);

  BeginTextureMode(tabletScreen);
  DrawRectangleRec
  (
    Rectangle
    {
      col * pixelDims.x,
      row * pixelDims.y,
      pixelDims.x,
      pixelDims.y
    },
    c.getCellColor()
  );
  EndTextureMode();
}

void tablet::generateCell(Vector2 coord, const textureMapping& texMap)
{
  const int W = (int)tabletDims.x;
  const int H = (int)tabletDims.y;
  int row0 = (int)coord.y;
  int col0 = (int)coord.x;

  cell& c = tabletPixels[row0][col0];
  if(!c.isSelected) { c.pickTile(texMap); }

  // Dequeue collapsed cell. Uses cellQueueCount so it's correct even when
  // pickSpecificTile was called externally before us (changing validIndexes first).
  int qc0 = cellQueueCount[row0][col0];
  if(qc0 >= 0)
  {
    entropyQueue.erase(std::make_tuple(qc0, row0, col0));
    cellQueueCount[row0][col0] = -1;
  }

  const int tileDims     = texMap.tileDims;
  const int centerOffset = (tileDims - 1) / 2;

  vector<vector<bool>> inQueue(H, vector<bool>(W, false));
  vector<vector<bool>> changed(H, vector<bool>(W, false));

  queue<Vector2> propQueue;
  propQueue.push(coord);
  inQueue[row0][col0] = true;

  while(!propQueue.empty())
  {
    Vector2 cur = propQueue.front();
    propQueue.pop();
    int curRow = (int)cur.y;
    int curCol = (int)cur.x;
    inQueue[curRow][curCol] = false;

    cell& curCell = tabletPixels[curRow][curCol];
    if(curCell.validIndexes.empty()) { continue; }

    for(int dy = -centerOffset; dy <= centerOffset; ++dy)
    {
      for(int dx = -centerOffset; dx <= centerOffset; ++dx)
      {
        if(dx == 0 && dy == 0) { continue; }
        int nx = curCol + dx;
        int ny = curRow + dy;
        if(nx < 0 || nx >= W || ny < 0 || ny >= H) { continue; }

        cell& neighbor = tabletPixels[ny][nx];
        if(neighbor.isSelected) { continue; }

        if(neighbor.updateTilesCompatibleWith(curCell, dx, dy, texMap))
        {
          int newCount = (int)neighbor.validIndexes.size();
          int qc = cellQueueCount[ny][nx];
          if(qc >= 0) entropyQueue.erase(std::make_tuple(qc, ny, nx));
          entropyQueue.insert(std::make_tuple(newCount, ny, nx));
          cellQueueCount[ny][nx] = newCount;

          changed[ny][nx] = true;
          if(!inQueue[ny][nx])
          {
            inQueue[ny][nx] = true;
            propQueue.push({(float)nx, (float)ny});
          }
        }
      }
    }
  }

  // Only recompute rough color for cells that actually had tiles eliminated.
  for(int r = 0; r < H; ++r)
    for(int col = 0; col < W; ++col)
      if(changed[r][col] && !tabletPixels[r][col].isSelected)
        tabletPixels[r][col].updateRoughColor(texMap);
}

void tablet::reset(const textureMapping& texMap)
{
  entropyQueue.clear();
  for(int row = 0; row < (int)tabletDims.y; ++row)
  {
    for(int col = 0; col < (int)tabletDims.x; ++col)
    {
      cell& c      = tabletPixels[row][col];
      c.isSelected = false;
      c.init(texMap);
      c.updateRoughColor(texMap);
      int count = (int)c.validIndexes.size();
      entropyQueue.insert(std::make_tuple(count, row, col));
      cellQueueCount[row][col] = count;
    }
  }
}

bool tablet::step(const textureMapping& texMap)
{
  while(!entropyQueue.empty())
  {
    int count = std::get<0>(*entropyQueue.begin());
    int row   = std::get<1>(*entropyQueue.begin());
    int col   = std::get<2>(*entropyQueue.begin());
    cell& c   = tabletPixels[row][col];

    // Stale entries (already selected or contradicted) get cleaned up here.
    if(c.isSelected || c.validIndexes.empty())
    {
      entropyQueue.erase(entropyQueue.begin());
      cellQueueCount[row][col] = -1;
      continue;
    }

    generateCell({(float)col, (float)row}, texMap);
    return true;
  }
  return false;
}

bool tablet::step(Vector2 coord, const textureMapping& texMap)
{
  generateCell(coord, texMap);
  return true;
}
