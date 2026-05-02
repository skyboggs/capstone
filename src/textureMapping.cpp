#include <iostream>
#include <vector>
#include "raylib.h"
#include "textureMapping.h"
#include "visUtils.h"
#include "tileUtils.h"
#include "tile.h"

using namespace std;


textureMapping::textureMapping(string imagePath, int tileSize)
  : 
    debugMode(false), 
    genRotatedTiles(false), 
    genMirroredX(false), 
    genMirroredY(false), 
    genOverlappingX(true), 
    genOverlappingY(true),
    usingOverlappingImage(false)

{
  tileDims = tileSize;

  try
  {
    image   = LoadImage(imagePath.c_str());

    imageDims = Vector2{(float)image.width,(float)image.height};

    if(genOverlappingX || genOverlappingY) { usingOverlappingImage = true; }

    if(usingOverlappingImage)
    {
      Image newImage;
      generateOverlappingImage(image,newImage);
      UnloadImage(image);

      image = ImageCopy(newImage);
      UnloadImage(newImage);
    }

    texture = LoadTextureFromImage(image);

    cout << "\033[0;32msuccessfully loaded image!\033[0;0m" << endl;
  } catch(...)
  {
    cout << "\033[0;31merror when loading image!\033[0;0m" << endl;
  }
};

textureMapping::~textureMapping()
{
  UnloadImage(image);
  UnloadImage(tileData);
  UnloadTexture(texture);
  UnloadTexture(tileAtlas);
}

void textureMapping::setNewTileSize(int newTileSize)
{
  tileDims = newTileSize;
};

void textureMapping::buildTileCache()
{
  tilePixels.resize(sourceRecs.size());
  for(int i = 0; i < (int)sourceRecs.size(); ++i)
  {
    const Rectangle& rec = sourceRecs[i];
    tilePixels[i].resize(tileDims * tileDims);
    for(int ly = 0; ly < tileDims; ++ly)
    {
      for(int lx = 0; lx < tileDims; ++lx)
      {
        tilePixels[i][ly * tileDims + lx] = GetImageColor(tileData, (int)rec.x + lx, (int)rec.y + ly);
      }
    }
  }
}





