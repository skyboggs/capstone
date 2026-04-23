#include <iostream>
#include <vector>
#include "raylib.h"
#include "tileUtils.h"
#include "tile.h"
#include "compatabilityVisTools.h"

using namespace std;


#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800


int main(int argc, const char** argv)
{
  Vector2 windowDims{WINDOW_WIDTH,WINDOW_HEIGHT};
	string imagePath = "../assets/Flowers.png";

	int tileWidth  = 3;
	int tileHeight = 3;

	InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"visualizing tiles");

  { // wrapping the below variables in a different scope so they deconstruct before CloseWindow call
// declaring our variables
  textureMapping genDetails(imagePath,Vector2{(float)tileWidth,(float)tileHeight});
  visualizerSettings visConfig;

/////////////////////////////////////////////////////
////// setting our variables and parameters /////////
/////////////////////////////////////////////////////

// setting genDetails variables
  //genDetails.image     = LoadImage(imagePath.c_str());
  //genDetails.texture   = LoadTextureFromImage(genDetails.image);
  //genDetails.setNewTileSize();

// setting genDetails Rectangles
  generateTileRecs(genDetails,windowDims);
  
// getting the dimensions of the window that the compatability visualizer is being drawn in
  const Rectangle& lastRec = genDetails.destRecs.at(genDetails.destRecs.size()-1);

  Vector2 displayDimensions
  {
    (lastRec.x + lastRec.width)  - genDetails.destRecs[0].x,
    (lastRec.y + lastRec.height) - genDetails.destRecs[0].y
  };

// initializiing our textures and drawing the background
  RenderTexture2D t1         = LoadRenderTexture(displayDimensions.x, displayDimensions.y);
  RenderTexture2D background = LoadRenderTexture(windowDims.x       , windowDims.y       );

  BeginTextureMode(background);
    ClearBackground(BLACK);
		drawCheckeredBackground(WINDOW_WIDTH,WINDOW_HEIGHT, 40,Color{0,170,100,100}, Color{0,200,100,100});
  EndTextureMode();

// marking a compatability list to change which tiles should be marked as compatible
  vector<bool> compatibleList(genDetails.destRecs.size(),false);

  try
  {
    for(int i=0;i<400;i+=(9 + ((i * i) % 7)))
    {
      compatibleList.at(i) = true;
    }
  }catch(out_of_range e)
  {
    cout << "out of range" << endl;
  }

  float currentAngle(0.0f);
  Vector2 drawLocation{(float)WINDOW_WIDTH/2.0f,(float)WINDOW_HEIGHT/2.0f};
  float adjustmentAmount = 1.5f;


//////////////////////////
//// draw loop ///////////
//////////////////////////
	while(!WindowShouldClose())
	{
    //printVec(genDetails.displayDims,"displayDims");
    if(IsKeyDown(KEY_LEFT )) { drawLocation.x -= adjustmentAmount; }
    if(IsKeyDown(KEY_RIGHT)) { drawLocation.x += adjustmentAmount; }
    if(IsKeyDown(KEY_UP   )) { drawLocation.y -= adjustmentAmount; }
    if(IsKeyDown(KEY_DOWN )) { drawLocation.y += adjustmentAmount; }
    if(IsKeyDown(KEY_A    )) { currentAngle   -= adjustmentAmount; }
    if(IsKeyDown(KEY_D    )) { currentAngle   += adjustmentAmount; }
  // checking for new user input
    updateConfig(visConfig);


// updating the compatability texture
    BeginTextureMode(t1);
      ClearBackground(BLANK);
      drawTileCompatabilities(visConfig,genDetails,compatibleList);
    EndTextureMode();

  // starting to draw
		BeginDrawing();
		  ClearBackground(BLACK);

  // drawing background
      DrawTexturePro
      (
        background.texture,
        (Rectangle){ 0, 0, (float)WINDOW_WIDTH, -(float)WINDOW_HEIGHT },
        (Rectangle){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
        Vector2{0,0},
        0.0f,
        WHITE
      );

  // drawing compatability visualizer
      DrawTexturePro
      (
        t1.texture,
        (Rectangle){ 0, 0, (float)displayDimensions.x, -(float)displayDimensions.y},
        (Rectangle){ (float)drawLocation.x, (float)drawLocation.y, 400, 400},
        Vector2{(float)200.0f,(float)200.0f},
        currentAngle,
        WHITE
      );

    //currentAngle += 0.2f;

		EndDrawing();
	}

  UnloadRenderTexture(t1);
  UnloadRenderTexture(background);

  }

	CloseWindow();

	return 0;
}


