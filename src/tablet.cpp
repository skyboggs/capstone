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
    tabletScreen(LoadRenderTexture((int)screenDimension.x, (int)screenDimension.y))
{
  for(int row = 0; row < (int)tabletDimensions.y; ++row)
  {
    for(int col = 0; col < (int)tabletDimensions.x; ++col)
    {
      generationQueue.push_back(Vector2{(float)col, (float)row});
    }
  }

  // Fisher-Yates shuffle
  for(int i = (int)generationQueue.size() - 1; i > 0; --i)
  {
    int j = rand() % (i + 1);
    Vector2 tmp         = generationQueue[i];
    generationQueue[i]  = generationQueue[j];
    generationQueue[j]  = tmp;
  }
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

void tablet::generateCell(const textureMapping& texMap)
{
  while(!generationQueue.empty())
  {
    Vector2 pos = generationQueue.back();
    generationQueue.pop_back();
    cell& c = tabletPixels[(int)pos.y][(int)pos.x];
    if(!c.isSelected)
    {
      c.pickTile(texMap);

      const int tileDims     = texMap.tileDims;
      const int centerOffset = (tileDims - 1) / 2;

      // inQueue[row][col] prevents a cell being enqueued twice in one propagation wave
      vector<vector<bool>> inQueue((int)tabletDims.y,
                                   vector<bool>((int)tabletDims.x, false));
      queue<Vector2> propQueue;
      propQueue.push(pos);
      inQueue[(int)pos.y][(int)pos.x] = true;

      while(!propQueue.empty())
      {
        Vector2 cur = propQueue.front();
        propQueue.pop();
        inQueue[(int)cur.y][(int)cur.x] = false;

        cell& curCell = tabletPixels[(int)cur.y][(int)cur.x];
        if(curCell.validIndexes.empty()) { continue; }

        for(int dy = -centerOffset; dy <= centerOffset; ++dy)
        {
          for(int dx = -centerOffset; dx <= centerOffset; ++dx)
          {
            if(dx == 0 && dy == 0) { continue; }
            int nx = (int)cur.x + dx;
            int ny = (int)cur.y + dy;
            if(nx < 0 || nx >= (int)tabletDims.x) { continue; }
            if(ny < 0 || ny >= (int)tabletDims.y) { continue; }

            cell& neighbor = tabletPixels[ny][nx];
            if(neighbor.isSelected) { continue; }

            if(neighbor.updateTilesCompatibleWith(curCell, dx, dy, texMap))
            {
              neighbor.updateRoughColor(texMap);
              if(!inQueue[ny][nx])
              {
                inQueue[ny][nx] = true;
                propQueue.push({(float)nx, (float)ny});
              }
            }
          }
        }
      }

      return;
    }
  }
}

void tablet::reset(const textureMapping& texMap)
{
  for(int row = 0; row < (int)tabletDims.y; ++row)
  {
    for(int col = 0; col < (int)tabletDims.x; ++col)
    {
      cell& c      = tabletPixels[row][col];
      c.isSelected = false;
      c.init(texMap);
      c.updateRoughColor(texMap);
    }
  }

  generationQueue.clear();
  for(int row = 0; row < (int)tabletDims.y; ++row)
    for(int col = 0; col < (int)tabletDims.x; ++col)
      generationQueue.push_back(Vector2{(float)col, (float)row});

  for(int i = (int)generationQueue.size() - 1; i > 0; --i)
  {
    int j              = rand() % (i + 1);
    Vector2 tmp        = generationQueue[i];
    generationQueue[i] = generationQueue[j];
    generationQueue[j] = tmp;
  }
}

void tablet::step(const textureMapping& texMap)
{
  cout << "\033[31m!! TODO : void tablet::step() !!\033[0m" << endl;
}
