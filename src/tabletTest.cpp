#include <iostream>
#include <cstdlib>
#include <ctime>
#include "raylib.h"
#include "tablet.h"
#include "textureMapping.h"
#include "visUtils.h"

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800

using namespace std;

int main(int argc, const char** argv)
{
  string imagePath = "../assets/Flowers.png";
  int    tileDim   = 3;

  vector<string> commandStack;

// pushing inputted arguments to the command stack
  for(int i=1;i<argc;++i)
  {
    commandStack.push_back(argv[i]);
    cout << "pushed: " << commandStack[commandStack.size()-1] << endl;
  }

// processing the command stack and setting program configuration
  for(int i=0;i<(int)commandStack.size();++i)
  {
    int remainingArgs = (commandStack.size() - i - 1);

    cout << "remaining args: " << (remainingArgs) << endl;
    cout << "[" << i << "]: '" << commandStack[i] << "'" << endl;

    if(commandStack[i].substr(0,2) == "--")
    {
      if(commandStack[i] == "--image")
      {
        if(remainingArgs >= 1)
        {
          checkCLA(commandStack[i], commandStack[i + 1], imagePath);
          ++i;
        }else
        {
          cout << "\033[31mNo inputted image\033[0m after --image flag!" << endl;
        }
      }else if(commandStack[i] == "--tile")
      {
        if(remainingArgs >= 1)
        {
          try
          {
            tileDim = stoi(commandStack[i + 1]);
            ++i;
          }catch(const invalid_argument& e)
          {
            cout << "\033[31mNot a valid\033[0m CLI number for the --tile command!" << endl;
          }
        }else
        {
          cout << "\033[31mError! You inputted " << remainingArgs << " dimensions after the --tile flag!\033[0m" << endl;
        }
      }
    }
  }

  srand((unsigned int)time(NULL));
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "tablet test");
  SetTargetFPS(60);

  Vector2 tabletDims  { 130.0f, 125.0f };
  Vector2 screenDims  { (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT };
  tablet  t(tabletDims, screenDims);

  textureMapping genDetails(imagePath, tileDim);
  genDetails.genOverlappingX = true;
  //genDetails.genOverlappingY = true;
  //genDetails.genMirroredX    = true;
  //genDetails.genMirroredY    = true;
  //genDetails.genRotatedTiles = true;
  generateTileRecs(genDetails, screenDims);

  t.reset(genDetails);
  t.updateTexture();

  int lastRow = -1;
  int lastCol = -1;

  while(!WindowShouldClose())
  {
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
      Vector2 mouse = GetMousePosition();
      int col = (int)(mouse.x / t.pixelDims.x);
      int row = (int)(mouse.y / t.pixelDims.y);
      if(col >= 0 && col < (int)tabletDims.x && row >= 0 && row < (int)tabletDims.y)
      {
        if(row != lastRow || col != lastCol)
        {
          t.tabletPixels[row][col].pickTile(genDetails);
          lastRow = row;
          lastCol = col;
        }
      }
    }
    else
    {
      lastRow = -1;
      lastCol = -1;
    }

    if(IsKeyPressed(KEY_G))
    {
      while(!t.generationQueue.empty())
        t.generateCell(genDetails);
    }

    if(IsKeyPressed(KEY_R))
    {
      bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
      t.reset(genDetails);
      if(!shift)
      {
        while(!t.generationQueue.empty())
          t.generateCell(genDetails);
      }
    }

    if(IsKeyDown(KEY_S) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))
    {
      t.generateCell(genDetails);
    }
    else if(IsKeyPressed(KEY_S))
    {
      t.generateCell(genDetails);
    }

    t.updateTexture();

    BeginDrawing();
      ClearBackground(BLACK);
      DrawTexturePro(
        t.tabletScreen.texture,
        Rectangle{0.0f, 0.0f, screenDims.x, -screenDims.y},
        Rectangle{0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT},
        Vector2{0.0f, 0.0f},
        0.0f,
        WHITE
      );
    EndDrawing();
  }

  UnloadRenderTexture(t.tabletScreen);
  CloseWindow();
  return 0;
}
